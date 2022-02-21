#include <cassert>
#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>
#include <chrono>
#include "CodeService/LuaFormatter.h"
#include "Util/format.h"
#include "Util/CommandLine.h"
#include "Util/StringUtil.h"
#include "CodeService/LuaEditorConfig.h"

using namespace std::chrono;

bool TestFormatted(std::string input, const std::string& shouldBe, std::shared_ptr<LuaCodeStyleOptions> options)
{
	auto parser = LuaParser::LoadFromBuffer(std::move(input));
	parser->BuildAstWithComment();

	LuaFormatter formatter(parser, *options);

	formatter.BuildFormattedElement();

	auto formattedText = formatter.GetFormattedText();

	return StringUtil::TrimSpace(formattedText) == StringUtil::TrimSpace(shouldBe);
}

uint64_t TestPerformance(std::string input, std::shared_ptr<LuaCodeStyleOptions> options)
{
	auto start = steady_clock::now();
	auto parser = LuaParser::LoadFromBuffer(std::move(input));
	parser->BuildAstWithComment();

	LuaFormatter formatter(parser, *options);

	formatter.BuildFormattedElement();

	volatile auto formattedText = formatter.GetFormattedText();

	return duration_cast<milliseconds>(steady_clock::now() - start).count();
}

bool TestGrammar(std::string input)
{
	auto parser = LuaParser::LoadFromBuffer(std::move(input));
	parser->BuildAstWithComment();
	bool result = parser->GetErrors().empty();

	if (!result)
	{
		for (auto& error : parser->GetErrors())
			std::cout << format("error: {}, from [{},{}] to [{},{}]", error.ErrorMessage,
			                    parser->GetLine(error.ErrorRange.StartOffset),
			                    parser->GetColumn(error.ErrorRange.StartOffset),
			                    parser->GetLine(error.ErrorRange.EndOffset),
			                    parser->GetColumn(error.ErrorRange.EndOffset)
			) << std::endl;
		return result;
	}

	auto options = std::make_shared<LuaCodeStyleOptions>();

	LuaFormatter formatter(parser, *options);

	formatter.BuildFormattedElement();

	auto formattedText = formatter.GetFormattedText();
	auto formattedParser = LuaParser::LoadFromBuffer(std::move(formattedText));

	formattedParser->BuildAstWithComment();
	result = formattedParser->GetErrors().empty();

	if (!result)
	{
		for (auto& error : formattedParser->GetErrors())
			std::cout << format("error after formatted: {}, from [{},{}] to [{},{}]", error.ErrorMessage,
			                    formattedParser->GetLine(error.ErrorRange.StartOffset),
			                    formattedParser->GetColumn(error.ErrorRange.StartOffset),
			                    formattedParser->GetLine(error.ErrorRange.EndOffset),
			                    formattedParser->GetColumn(error.ErrorRange.EndOffset)
			) << std::endl;
	}

	return result;
}

void CollectLuaFile(std::filesystem::path directoryPath, std::vector<std::string>& paths, std::filesystem::path& root)
{
	if (!std::filesystem::exists(directoryPath))
	{
		return;
	}

	for (auto& it : std::filesystem::directory_iterator(directoryPath))
	{
		if (std::filesystem::is_directory(it.status()))
		{
			CollectLuaFile(it.path().string(), paths, root);
		}
		else if (it.path().extension() == ".lua")
		{
			paths.push_back(std::filesystem::relative(it.path(), root).string());
		}
	}
}

std::string ReadFile(const std::string& path)
{
#ifdef _WIN32
	std::fstream fin(path, std::ios::in | std::ios::binary);
#else
	std::fstream fin(path, std::ios::in);
#endif

	if (fin.is_open())
	{
		std::stringstream s;
		s << fin.rdbuf();
		return s.str();
	}

	return "";
}


int main(int argc, char* argv[])
{
	CommandLine commandLine;

	commandLine.AddTarget("CheckGrammar");
	commandLine.AddTarget("CheckFormatResult");
	commandLine.AddTarget("CheckFormatResultByOption");
	commandLine.AddTarget("Performance");

	commandLine.Add<std::string>("work-directory", "w", "special base work directory");
	commandLine.Add<std::string>("formatted-work-directory", "f", "special formatted work directory");

	if (!commandLine.Parse(argc, argv))
	{
		commandLine.PrintUsage();
		return -1;
	}

	std::filesystem::path workRoot(commandLine.Get<std::string>("work-directory"));
	std::vector<std::string> luaFiles;

	CollectLuaFile(workRoot, luaFiles, workRoot);

	auto target = commandLine.GetTarget();
	bool success = true;

	if (target == "CheckFormatResult")
	{
		auto options = std::make_shared<LuaCodeStyleOptions>();
		std::filesystem::path formattedRoot(commandLine.Get<std::string>("formatted-work-directory"));
		for (auto& path : luaFiles)
		{
			auto waitFormattingFilePath = workRoot / path;
			auto waitFormattingText = ReadFile(waitFormattingFilePath.string());

			auto formattedFilePath = formattedRoot / path;

			auto formattedText = ReadFile(formattedFilePath.string());

			bool passed = TestFormatted(waitFormattingText, formattedText, options);

			success &= passed;
			std::cout << format("test format {} ... {}", path, passed ? "passed" : "false") << std::endl;

			passed = TestFormatted(formattedText, formattedText, options);
			success &= passed;
			std::cout << format("test format stability {} ... {}", path, passed ? "passed" : "false") << std::endl;
		}
	}
	else if (target == "CheckFormatResultByOption")
	{
		auto editorconfigPath = workRoot / ".editorconfig";
		auto editorConfig = LuaEditorConfig::LoadFromFile(editorconfigPath.string());
		if (!editorConfig)
		{
			std::cerr << "can not find editorconfig file" << std::endl;
			return -1;
		}
		editorConfig->SetWorkspace(workRoot.string());
		std::filesystem::path formattedRoot(commandLine.Get<std::string>("formatted-work-directory"));
		for (auto& path : luaFiles)
		{
			auto options = editorConfig->Generate(path);
			auto waitFormattingFilePath = workRoot / path;
			auto waitFormattingText = ReadFile(waitFormattingFilePath.string());

			auto formattedFilePath = formattedRoot / path;

			auto formattedText = ReadFile(formattedFilePath.string());

			bool passed = TestFormatted(waitFormattingText, formattedText, options);

			success &= passed;
			std::cout << format("test format {} ... {}", path, passed ? "passed" : "false") << std::endl;

			passed = TestFormatted(formattedText, formattedText, options);
			success &= passed;
			std::cout << format("test format stability {} ... {}", path, passed ? "passed" : "false") << std::endl;
		}
	}
	else if (target == "CheckGrammar")
	{
		for (auto& path : luaFiles)
		{
			auto filePath = workRoot / path;
			auto text = ReadFile(filePath.string());

			bool passed = TestGrammar(text);

			success &= passed;
			std::cout << format("test check grammar {} ... {}", path, passed ? "passed" : "false") << std::endl;
		}
	}
	else if(target == "Performance")
	{
		auto options = std::make_shared<LuaCodeStyleOptions>();
		for (auto& path : luaFiles)
		{
			auto waitFormattingFilePath = workRoot / path;
			auto waitFormattingText = ReadFile(waitFormattingFilePath.string());

			auto time = TestPerformance(waitFormattingText, options);
			std::cout << format("test format Performance {} ...it cost {}ms", path, time ) << std::endl;
		}
	}


	return success ? 0 : -1;
}

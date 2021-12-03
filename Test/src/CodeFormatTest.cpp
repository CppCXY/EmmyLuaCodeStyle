﻿#include <cassert>
#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>
#include "CodeService/LuaFormatter.h"
#include "Util/format.h"
#include "Util/CommandLine.h"

std::string_view TrimSpace(std::string_view source)
{
	if (source.empty())
	{
		return source;
	}

	std::size_t start = 0;
	for (; start != source.size(); start++)
	{
		char ch = source[start];
		if (ch != '\n' && ch != '\r' && ch != ' ')
		{
			break;
		}
	}

	int end = source.size() - 1;
	for (; end >= 0; end--)
	{
		char ch = source[end];
		if (ch != '\n' && ch != '\r' && ch != ' ')
		{
			break;
		}
	}

	if (end < static_cast<int>(start))
	{
		return "";
	}
	return source.substr(start, end - start + 1);
}

bool TestFormatted(std::string input, const std::string& shouldBe, std::shared_ptr<LuaCodeStyleOptions> options)
{
	auto parser = LuaParser::LoadFromBuffer(std::move(input));
	parser->BuildAstWithComment();

	LuaFormatter formatter(parser, *options);

	formatter.BuildFormattedElement();

	auto formattedText = formatter.GetFormattedText();

	return TrimSpace(formattedText) == TrimSpace(shouldBe);
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
	std::fstream fin(path, std::ios::in | std::ios::binary);

	if (fin.is_open())
	{
		std::stringstream s;
		s << fin.rdbuf();
		return s.str();
	}

	return "";
}

// 第一个参数是待格式化文本目录，第二个参数是格式化预期结果
int main(int argc, char* argv[])
{
	CommandLine commandLine;

	commandLine.AddTarget("CheckGrammar");
	commandLine.AddTarget("CheckFormatResult");

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
			std::cout << format("test format {} ... {}", path, passed ? "true" : "false") << std::endl;
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
			std::cout << format("test check grammar {} ... {}", path, passed? "true" : "false") << std::endl;
		}
	}


	return success ? 0 : -1;
}
#include "LuaFormat.h"

#include <fstream>

#include "Util/format.h"
#include <iostream>

#include "CodeService/LuaEditorConfig.h"
#include "CodeService/LuaFormatter.h"
#include "CodeService/NameStyle/NameStyleChecker.h"
#include "Util/StringUtil.h"

LuaFormat::LuaFormat()
	: _options(nullptr),
	  _parser(nullptr)
{
}

bool LuaFormat::SetInputFile(std::string_view input)
{
	_inputFile = input;
	_parser = LuaParser::LoadFromFile(input);
	return _parser != nullptr;
}

bool LuaFormat::ReadFromStdin(std::size_t size)
{
	std::string buffer;
	buffer.resize(size);
	std::cin.get(buffer.data(), size, EOF);
	auto realSize = strnlen(buffer.data(), size);
	buffer.resize(realSize);
	_parser = LuaParser::LoadFromBuffer(std::move(buffer));
	return _parser != nullptr;
}

void LuaFormat::SetOutputFile(std::string_view path)
{
	_outFile = std::string(path);
}

bool IsSubRelative(std::filesystem::path& path, std::filesystem::path base)
{
	auto relative = std::filesystem::relative(path, base);
	auto relativeString = relative.string();
	if (relativeString.empty())
	{
		return false;
	}
	if (relativeString == ".")
	{
		return true;
	}
	return !relativeString.starts_with(".");
}

void LuaFormat::AutoDetectConfig(std::filesystem::path workspace)
{
	if (_inputFile.empty())
	{
		return;
	}
	std::filesystem::path inputPath(_inputFile);
	if (!IsSubRelative(inputPath, workspace))
	{
		return;
	}

	auto directory = absolute(inputPath.parent_path());
	while (IsSubRelative(directory, workspace))
	{
		auto editorconfigPath = directory / ".editorconfig";
		if (std::filesystem::exists(editorconfigPath))
		{
			SetConfigPath(editorconfigPath.string());
			return;
		}

		directory = directory.parent_path();
	}
}

void LuaFormat::SetConfigPath(std::string_view config)
{
	if (!config.empty())
	{
		auto editorConfig = LuaEditorConfig::LoadFromFile(std::string(config));
		if (editorConfig)
		{
			_options = editorConfig->Generate(_inputFile);
		}
	}
}

void LuaFormat::SetOptions(std::shared_ptr<LuaCodeStyleOptions> options)
{
	_options = options;
}

void LuaFormat::SetDefaultOptions(std::map<std::string, std::string, std::less<>>& keyValues)
{
	_defaultOptions.swap(keyValues);
}

bool LuaFormat::Reformat()
{
	_parser->BuildAstWithComment();

	if (_parser->HasError())
	{
		return false;
	}

	if (_options == nullptr)
	{
		_options = std::make_shared<LuaCodeStyleOptions>();
		if (!_defaultOptions.empty())
		{
			LuaEditorConfig::ParseFromSection(_options, _defaultOptions);
		}
	}

	if (_outFile.empty())
	{
		_options->end_of_line = EndOfLine::LF;
	}

	LuaFormatter formatter(_parser, *_options);
	formatter.BuildFormattedElement();

	auto formattedText = formatter.GetFormattedText();
	if (!_outFile.empty())
	{
		std::fstream f(_outFile, std::ios::out | std::ios::binary);
		f.write(formattedText.data(), formattedText.size());
		f.close();
	}
	else
	{
		std::cout.write(formattedText.data(), formattedText.size());
	}
	return true;
}

bool LuaFormat::Check(std::string_view workspace, std::shared_ptr<CodeSpellChecker> spellChecker)
{
	_parser->BuildAstWithComment();

	std::string_view inputFile = _inputFile;
	if (!workspace.empty())
	{
		inputFile = StringUtil::GetFileRelativePath(workspace, inputFile);
	}
	if (_parser->HasError())
	{
		auto errors = _parser->GetErrors();

		std::cout << Util::format("Check {}\t{} error", inputFile, errors.size()) << std::endl;
		auto luaFile = _parser->GetLuaFile();
		for (auto& error : errors)
		{
			DiagnosisInspection(error.ErrorMessage, error.ErrorRange, luaFile, inputFile);
		}

		return false;
	}

	if (_options == nullptr)
	{
		_options = std::make_shared<LuaCodeStyleOptions>();
		if (!_defaultOptions.empty())
		{
			LuaEditorConfig::ParseFromSection(_options, _defaultOptions);
		}
	}

	LuaFormatter formatter(_parser, *_options);
	formatter.BuildFormattedElement();

	DiagnosisContext ctx(_parser, *_options);
	formatter.CalculateDiagnosisInfos(ctx);

	if (_options->enable_check_codestyle)
	{
		NameStyleChecker styleChecker(ctx);
		styleChecker.Analysis();
	}
	if (spellChecker)
	{
		spellChecker->Analysis(ctx);
	}

	ctx.DiagnoseLine();

	auto diagnosis = ctx.GetDiagnosisInfos();
	if (!diagnosis.empty())
	{
		std::cout << Util::format("Check {}\t{} warning", inputFile, diagnosis.size()) << std::endl;

		for (auto& d : diagnosis)
		{
			auto luaFile = _parser->GetLuaFile();

			DiagnosisInspection(d.Message, TextRange(
				                    luaFile->GetOffsetFromPosition(d.Range.Start.Line, d.Range.Start.Character),
				                    luaFile->GetOffsetFromPosition(d.Range.End.Line, d.Range.End.Character)
			                    ), luaFile, inputFile);
		}

		return false;
	}
	std::cout << Util::format("Check {} OK", inputFile) << std::endl;
	return true;
}

void LuaFormat::DiagnosisInspection(std::string_view message, TextRange range, std::shared_ptr<LuaFile> file,
                                    std::string_view path)
{
	std::string_view source = file->GetSource();
	auto startLine = file->GetLine(range.StartOffset);
	auto startChar = file->GetColumn(range.StartOffset);
	auto endLine = file->GetLine(range.EndOffset);
	auto endChar = file->GetColumn(range.EndOffset);
	std::cout << Util::format("\t{}({}:{} to {}:{}): {}", path, startLine + 1, startChar, endLine + 1, endChar,
	                    message) << std::endl;
}

#pragma once

#include <string>
#include <string_view>
#include <filesystem>
#include <cstring>

#include "LuaParser/LuaParser.h"
#include "CodeService/Config/LuaCodeStyleOptions.h"
#include "CodeService/Diagnosis/LuaDiagnosisInfo.h"
#include "CodeService/Spell/CodeSpellChecker.h"

class LuaFormat
{
public:
	LuaFormat();

	bool SetInputFile(std::string_view input);

	bool ReadFromStdin(std::size_t size);

	void SetOutputFile(std::string_view path);

	void AutoDetectConfig(std::filesystem::path workspace = std::filesystem::current_path());

	void SetConfigPath(std::string_view config);

	void SetOptions(std::shared_ptr<LuaCodeStyleOptions> options);

	void SetDefaultOptions(std::map<std::string, std::string, std::less<>>& keyValues);

	bool Reformat();

	bool Check(std::string_view workspace = "", std::shared_ptr<CodeSpellChecker> checker = nullptr);

	void DiagnosisInspection(std::string_view message, TextRange range, std::shared_ptr<LuaFile> file, std::string_view path);
private:
	std::string _inputFile;
	std::string _outFile;
	std::shared_ptr<LuaCodeStyleOptions> _options;
	std::shared_ptr<LuaParser> _parser;
	std::map<std::string, std::string, std::less<>> _defaultOptions;
};

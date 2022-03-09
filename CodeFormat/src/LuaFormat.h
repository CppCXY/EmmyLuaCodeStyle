#pragma once

#include <string>
#include <string_view>
#include <filesystem>
#include <cstring>

#include "LuaParser/LuaParser.h"
#include "CodeService/LuaCodeStyleOptions.h"

class LuaFormat
{
public:
	LuaFormat();

	bool SetInputFile(std::string_view input);

	bool ReadFromStdin(std::size_t size);

	void SetOutputFile(std::string_view path);

	void AutoDetectConfigRoot(std::string_view root);

	void SetConfigPath(std::string_view config);

	void SetDefaultOptions(std::map<std::string, std::string,std::less<>>& keyValues);

	bool Reformat();

	bool Check();

private:
	std::string _inputFile;
	std::string _outFile;
	std::shared_ptr<LuaCodeStyleOptions> _options;
	std::shared_ptr<LuaParser> _parser;
	std::map<std::string, std::string, std::less<>> _defaultOptions;
};

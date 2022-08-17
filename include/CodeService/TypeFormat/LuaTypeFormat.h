#pragma once

#include "CodeService/LuaFormatter.h"
#include "CodeService/LuaFormatRange.h"
#include "LuaTypeFormatOptions.h"

class LuaTypeFormat
{
public:
	struct Result
	{
		LuaFormatRange Range = LuaFormatRange(0, 0);

		std::string Text;
	};


	LuaTypeFormat(std::shared_ptr<LuaParser> luaParser, LuaCodeStyleOptions& options, LuaTypeFormatOptions& typeOptions);
	~LuaTypeFormat() = default;

	void Analysis(std::string_view trigger, int line, int character);

	bool HasFormatResult();

	Result& GetResult();

private:
	void AnalysisReturn(int line, int character);
	void CompleteMissToken(int line, int character, LuaError& luaError);
	void FormatLine(int line);

	std::shared_ptr<LuaParser> _parser;
	LuaCodeStyleOptions& _options;
	LuaTypeFormatOptions& _typeOptions;
	bool _hasResult;
	Result _result;
};

#pragma once

#include "LuaFormatter.h"
#include "LuaFormatRange.h"

class LuaTypeFormat
{
public:
	struct Result
	{
		LuaFormatRange Range = LuaFormatRange(0, 0);

		std::string Text;
	};


	LuaTypeFormat(std::shared_ptr<LuaParser> luaParser, LuaCodeStyleOptions& options);
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
	bool _hasResult;
	Result _result;
};

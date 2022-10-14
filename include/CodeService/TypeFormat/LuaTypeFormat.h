#pragma once

#include "CodeService/LuaFormatter.h"
#include "CodeService/RangeFormat/LuaFormatRange.h"
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

	std::vector<Result>& GetResult();

private:
	void AnalysisReturn(int line, int character);
	void CompleteMissToken(int line, int character, LuaError& luaError);
	void FormatLine(int line);
	void FixIndent(int line, int character);

	void FixEndIndent(int line, int character);

	std::shared_ptr<LuaParser> _parser;
	LuaCodeStyleOptions& _options;
	LuaTypeFormatOptions& _typeOptions;
	bool _hasResult;
	std::vector<Result> _results;
};

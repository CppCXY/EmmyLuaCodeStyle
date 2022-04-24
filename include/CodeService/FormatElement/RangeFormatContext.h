#pragma once

#include "SerializeContext.h"
#include "CodeService/LuaFormatRange.h"

class RangeFormatContext : public SerializeContext
{
public:
	RangeFormatContext(std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options, LuaFormatRange validRange);

	void Print(std::string_view text, TextRange range) override;

	void Print(char ch, int Offset) override;

	void PrintIndentOnly(int line) override;

	void PrintBlank(int blank) override;

	void PrintLine(int line) override;

	std::string GetText() override;

	LuaFormatRange GetFormattedRange();
private:
	LuaFormatRange _validRange;
	LuaFormatRange _formattedRange;
	bool _inValidRange;
};

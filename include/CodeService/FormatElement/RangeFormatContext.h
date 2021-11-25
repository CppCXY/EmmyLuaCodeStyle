#pragma once

#include "FormatContext.h"
#include "CodeService/LuaFormatRange.h"

class RangeFormatContext : public FormatContext
{
public:
	RangeFormatContext(std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options, LuaFormatRange validRange);
	void Print(TextElement& textElement) override;
	void PrintBlank(int blank) override;
	void PrintLine(int line) override;
	std::string GetText() override;

	LuaFormatRange GetFormattedRange();
private:
	LuaFormatRange _validRange;
	LuaFormatRange _formattedRange;
	bool _inValidRange;
};

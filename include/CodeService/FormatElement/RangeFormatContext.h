#pragma once

#include "SerializeContext.h"
#include "CodeService/LuaFormatRange.h"

class RangeFormatContext : public SerializeContext
{
public:
	RangeFormatContext(std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options, LuaFormatRange validRange);
	void Print(TextElement& textElement) override;
	void PrintBlank(std::size_t blank) override;
	void PrintLine(std::size_t line) override;
	std::string GetText() override;

	LuaFormatRange GetFormattedRange();
private:
	LuaFormatRange _validRange;
	LuaFormatRange _formattedRange;
	bool _inValidRange;
};

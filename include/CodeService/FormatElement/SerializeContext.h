#pragma once

#include "FormatContext.h"

class SerializeContext : public FormatContext
{
public:
	SerializeContext(std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options);

	virtual void Print(std::string_view text, TextRange range);

	virtual void Print(char ch, int Offset);

	virtual void PrintLine(int line);

	virtual void PrintBlank(int blank);

	virtual std::string GetText();

	void SetReadySize(std::size_t size);
protected:
	void PrintIndent(std::size_t indent, IndentStyle style);
	std::string _buffer;
};


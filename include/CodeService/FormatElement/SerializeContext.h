#pragma once

#include "FormatContext.h"

class TextElement;

class SerializeContext : public FormatContext
{
public:
	SerializeContext(std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options);

	virtual void Print(TextElement& textElement);

	virtual void PrintLine(int line);

	virtual void PrintBlank(int blank);

	virtual std::string GetText();

	void SetReadySize(std::size_t size);
protected:
	void PrintIndent(std::size_t indent, IndentStyle style);
	std::string _buffer;
};


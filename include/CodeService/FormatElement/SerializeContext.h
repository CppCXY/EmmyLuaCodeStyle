#pragma once

#include "FormatContext.h"

class TextElement;

class SerializeContext : public FormatContext
{
public:
	SerializeContext(std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options);

	virtual void Print(TextElement& textElement);

	virtual void PrintLine(std::size_t line);

	virtual void PrintBlank(std::size_t blank);

	virtual std::string GetText();
protected:
	void PrintIndent(std::size_t indent, IndentStyle style);
	std::stringstream _os;
};


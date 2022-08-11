#pragma once

#include "FormatContext.h"

class SerializeContext : public FormatContext
{
public:
	SerializeContext(std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options);

	virtual void Print(std::string_view text, TextRange range = TextRange());

	virtual void Print(char ch, int Offset);

	virtual void PrintLine(int line);

	virtual void PrintBlank(int blank);

	virtual std::string GetText();

	virtual void PrintIndentOnly(int line  = -1);

	void SetReadySize(std::size_t size);
protected:
	static void InnerPrintEndOfLine(std::string& buffer, EndOfLine endOfLine);

	void PrintIndent(std::size_t indent, IndentStyle style);
	void InnerPrintText(std::string_view text, TextRange range);
	void PrintEndOfLine();

	std::string _buffer;
	std::shared_ptr<LuaFile> _luaFile;
};

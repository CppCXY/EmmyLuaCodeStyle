#pragma once
#include <string_view>
#include <sstream>
#include <vector>
#include <stack>
#include <map>
#include "CodeService/LuaFormatOptions.h"
#include "LuaParser/LuaParser.h"

class FormatContext
{
public:
	struct IndentPair
	{
		int Indent;
		std::string IndentString;
	};

	FormatContext(std::shared_ptr<LuaParser> parser ,LuaFormatOptions& options);

	void Print(std::string_view text);

	void PrintLine(int line);

	void PrintBlank(int blank);
	// void
	void AddIndent();

	void RecoverIndent();

	int GetLine(int offset);

	int GetColumn(int offset);

	std::size_t GetCharacterCount() const;

	std::string GetText();

private:
	std::stack<IndentPair, std::vector<IndentPair>> _indentStack;
	std::map<int, std::string> _indentMap;
	std::stringstream _os;
	LuaFormatOptions _options;

	std::size_t _characterCount;
	std::shared_ptr<LuaParser> _parser;
};

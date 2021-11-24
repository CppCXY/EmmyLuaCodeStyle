#pragma once
#include <string_view>
#include <sstream>
#include <vector>
#include <stack>
#include <map>
#include "CodeService/LuaCodeStyleOptions.h"
#include "LuaParser/LuaParser.h"

class TextElement;

class FormatContext
{
public:
	struct IndentState
	{
		int Indent = 0;
		std::string IndentString = "";
	};

	FormatContext(std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options);
	virtual ~FormatContext();

	virtual void Print(TextElement& textElement);

	virtual void PrintLine(int line);

	virtual void PrintBlank(int blank);

	void AddIndent(int specialIndent = -1);

	void RecoverIndent();

	int GetLine(int offset);

	int GetColumn(int offset);

	std::size_t GetCharacterCount() const;

	std::size_t GetCurrentIndent() const;

	std::string GetText();

	std::shared_ptr<LuaParser> GetParser();

protected:
	void PrintIndent(int indent, const std::string& indentString);

	std::stack<IndentState, std::vector<IndentState>> _indentStack;
	std::map<int, std::string> _indentMap;
	std::stringstream _os;
	LuaCodeStyleOptions& _options;

	std::size_t _characterCount;
	std::shared_ptr<LuaParser> _parser;
};

#pragma once
#include <string_view>
#include <sstream>
#include <vector>
#include <stack>
#include <map>
#include "CodeService/LuaFormatOptions.h"
#include "LuaParser/LuaParser.h"
#include "CodeService/LuaDiagnosisInfo.h"

class FormatContext
{
public:
	struct IndentState
	{
		int Indent = 0;
		std::string IndentString = "";
	};

	FormatContext(std::shared_ptr<LuaParser> parser, LuaFormatOptions& options);

	void Print(std::string_view text);

	void PrintLine(int line);

	void PrintBlank(int blank);

	void PrintIndent(int indent, const std::string& indentString);

	void AddIndent(int specialIndent = -1);

	void RecoverIndent();

	int GetLine(int offset);

	int GetColumn(int offset);

	std::size_t GetCharacterCount() const;

	std::size_t GetCurrentIndent() const;

	std::string GetText();

	std::shared_ptr<LuaParser> GetParser();

private:
	std::stack<IndentState, std::vector<IndentState>> _indentStack;
	std::map<int, std::string> _indentMap;
	std::stringstream _os;
	LuaFormatOptions& _options;

	std::size_t _characterCount;
	std::shared_ptr<LuaParser> _parser;
};

#pragma once
#include <string_view>
#include <sstream>
#include <vector>
#include <stack>
#include <map>
#include "CodeService/LuaCodeStyleOptions.h"
#include "LuaParser/LuaParser.h"

class FormatContext
{
public:
	struct IndentState
	{
		std::size_t Indent = 0;
		IndentStyle IndentStyle = IndentStyle::Space;
	};

	FormatContext(std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options);
	virtual ~FormatContext();

	void AddIndent();

	void AddIndent(std::size_t specialIndent, IndentStyle style);

	void RecoverIndent();

	int GetLine(int offset);

	int GetColumn(int offset);

	std::size_t GetCharacterCount() const;

	std::size_t GetCurrentIndent() const;

	std::size_t GetLastIndent() const;

	std::shared_ptr<LuaParser> GetParser();

	LuaCodeStyleOptions& GetOptions();

protected:
	// 不用std::stack
	std::vector<IndentState> _indentStack;
	std::map<std::size_t, std::string> _indentMap;

	LuaCodeStyleOptions& _options;

	std::size_t _characterCount;
	std::shared_ptr<LuaParser> _parser;
};

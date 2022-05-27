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
		std::size_t SpaceIndent = 0;
		std::size_t TabIndent = 0;
		IndentStyle Style = IndentStyle::Space;
	};

	FormatContext(std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options);
	virtual ~FormatContext();

	void AddIndent();

	void AddIndent(IndentState state);

	void RecoverIndent();

	int GetLine(int offset);

	int GetColumn(int offset);

	IndentState CalculateIndentState(int offset);

	std::size_t GetCharacterCount() const;

	IndentState GetCurrentIndent() const;

	IndentState GetLastIndent() const;

	bool OnlyEmptyCharBefore(int offset);

	bool ShouldBreakLine(TextRange range);

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

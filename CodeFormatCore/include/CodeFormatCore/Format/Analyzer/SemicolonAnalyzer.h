#pragma once

#include "FormatAnalyzer.h"
#include "FormatStrategy.h"
#include <unordered_map>
#include <unordered_set>

class SemicolonAnalyzer : public FormatAnalyzer {
public:
	DECLARE_FORMAT_ANALYZER(SemicolonAnalyzer)

	SemicolonAnalyzer();

	void Analyze(FormatState& f, const LuaSyntaxTree& t) override;

	void Query(FormatState& f, LuaSyntaxNode syntaxNode, const LuaSyntaxTree& t, FormatResolve& resolve) override;

private:
	void AddSemicolon(LuaSyntaxNode n, const LuaSyntaxTree& t);
	void InsertNewLineBeforeNode(LuaSyntaxNode n, const LuaSyntaxTree& t);
	void RemoveSemicolon(LuaSyntaxNode n, const LuaSyntaxTree& t);
	bool IsFirstStmtOfLine(LuaSyntaxNode n, const LuaSyntaxTree& t);
	bool IsLastStmtOfLine(LuaSyntaxNode n, const LuaSyntaxTree& t);
	bool EndsWithSemicolon(LuaSyntaxNode n, const LuaSyntaxTree& t);
	LuaSyntaxNode GetLastNonCommentToken(LuaSyntaxNode n, const LuaSyntaxTree& t);

	std::unordered_map<std::size_t, SemicolonStrategy> _semicolon;
};
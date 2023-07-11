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

	std::unordered_set<std::size_t> _semicolon;
};
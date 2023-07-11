#include "CodeFormatCore/Format/Analyzer/SemicolonAnalyzer.h"
#include "CodeFormatCore/Format/FormatState.h"
#include "CodeFormatCore/Config/LuaStyleEnum.h"
#include <iostream>

SemicolonAnalyzer::SemicolonAnalyzer() {

}

void SemicolonAnalyzer::Analyze(FormatState& f, const LuaSyntaxTree& t) {
	//if (f.GetStyle().end_statement_with_semicolon == EndStmtWithSemicolon::Keep) {
	//	return; // No analysis needed
	//}

	for (auto syntaxNode : t.GetSyntaxNodes()) {
		if (syntaxNode.IsNode(t)) {
			if (detail::multi_match::StatementMatch(syntaxNode.GetSyntaxKind(t))) {


				AddSemicolon(syntaxNode, t);
			}
		}
	}
}

void SemicolonAnalyzer::Query(FormatState& f, LuaSyntaxNode n, const LuaSyntaxTree& t, FormatResolve& resolve) {
	if (_semicolon.find(n.GetIndex()) != _semicolon.end()) {
		resolve.SetTokenStrategy(TokenStrategy::StmtEndSemicolon);
		resolve.SetTokenAddStrategy(TokenAddStrategy::StmtEndSemicolon);
	}
}

void SemicolonAnalyzer::AddSemicolon(LuaSyntaxNode n, const LuaSyntaxTree& t) {
	auto token = n.GetLastToken(t);
	if (token.IsToken(t)) {
		_semicolon.insert(n.GetLastToken(t).GetIndex());
	}
}
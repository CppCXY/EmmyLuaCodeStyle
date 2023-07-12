#include "CodeFormatCore/Format/Analyzer/SemicolonAnalyzer.h"
#include "CodeFormatCore/Format/FormatState.h"
#include "CodeFormatCore/Config/LuaStyleEnum.h"
#include <iostream>

SemicolonAnalyzer::SemicolonAnalyzer() {

}

void SemicolonAnalyzer::Analyze(FormatState& f, const LuaSyntaxTree& t) {
	if (f.GetStyle().end_statement_with_semicolon == EndStmtWithSemicolon::Keep) {
		return; // No analysis needed
	}

	for (auto syntaxNode : t.GetSyntaxNodes()) {
		if (syntaxNode.IsNode(t)) {
			if (detail::multi_match::StatementMatch(syntaxNode.GetSyntaxKind(t))) {
				switch (f.GetStyle().end_statement_with_semicolon) {
					case EndStmtWithSemicolon::Always:
						if (!EndsWithSemicolon(syntaxNode, t)) {
							AddSemicolon(syntaxNode, t);
						}
						break;
					case EndStmtWithSemicolon::Never:
						if (EndsWithSemicolon(syntaxNode, t)) {
							if (IsSingleLineStmt(syntaxNode, t)) {
								RemoveSemicolon(syntaxNode, t);
							}
							else {
								ReplaceSemicolonWithNewLine(syntaxNode, t);
							}
						}
						break;
					case EndStmtWithSemicolon::SameLine:
						if (EndsWithSemicolon(syntaxNode, t)) {
							if (IsSingleLineStmt(syntaxNode, t)) {
								RemoveSemicolon(syntaxNode, t);
							}
						}
						break;
					default:
						break;
				}


				AddSemicolon(syntaxNode, t);
			}
		}
	}
}

void SemicolonAnalyzer::Query(FormatState& f, LuaSyntaxNode n, const LuaSyntaxTree& t, FormatResolve& resolve) {
	auto it = _semicolon.find(n.GetIndex());
	if (it != _semicolon.end()) {
		auto& strategy = it->second;
		switch (strategy) {
		case SemicolonStrategy::Add:
			resolve.SetTokenAddStrategy(TokenAddStrategy::StmtEndSemicolon);
			break;
		case SemicolonStrategy::Remove:
			resolve.SetTokenStrategy(TokenStrategy::StmtEndSemicolon);
			break;
		case SemicolonStrategy::ReplaceWithNewLine:
			resolve.SetTokenStrategy(TokenStrategy::StmtEndSemicolonNewLine);
			break;
		default:
			break;
		}
	}
}

void SemicolonAnalyzer::AddSemicolon(LuaSyntaxNode n, const LuaSyntaxTree& t) {
	auto token = n.GetLastToken(t);
	if (token.IsToken(t)) {
		_semicolon[token.GetIndex()] = SemicolonStrategy::Add;
	}
}

void SemicolonAnalyzer::ReplaceSemicolonWithNewLine(LuaSyntaxNode n, const LuaSyntaxTree& t) {
	auto token = n.GetLastToken(t);
	if (token.IsToken(t)) {
		_semicolon[token.GetIndex()] = SemicolonStrategy::ReplaceWithNewLine;
	}
}

void SemicolonAnalyzer::RemoveSemicolon(LuaSyntaxNode n, const LuaSyntaxTree& t) {
	auto token = n.GetLastToken(t);
	if (token.IsToken(t)) {
		_semicolon[token.GetIndex()] = SemicolonStrategy::Remove;
	}
}

bool SemicolonAnalyzer::IsSingleLineStmt(LuaSyntaxNode n, const LuaSyntaxTree& t) {
	return false;
}

bool SemicolonAnalyzer::EndsWithSemicolon(LuaSyntaxNode n, const LuaSyntaxTree& t) {
	auto token = n.GetLastToken(t);
	auto text = token.GetText(t);

	return text == ";";
}
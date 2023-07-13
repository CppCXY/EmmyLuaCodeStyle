#include "CodeFormatCore/Format/Analyzer/SemicolonAnalyzer.h"
#include "CodeFormatCore/Format/FormatState.h"
#include "CodeFormatCore/Config/LuaStyleEnum.h"
#include <iostream>
#include <LuaParser/Lexer/LuaTokenTypeDetail.h>

SemicolonAnalyzer::SemicolonAnalyzer() {

}

void SemicolonAnalyzer::Analyze(FormatState& f, const LuaSyntaxTree& t) {
	if (f.GetStyle().end_statement_with_semicolon == EndStmtWithSemicolon::Keep) {
		return; // No analysis needed
	}

	for (auto syntaxNode : t.GetSyntaxNodes()) {
		if (syntaxNode.IsNode(t)) {
			if (detail::multi_match::StatementMatch(syntaxNode.GetSyntaxKind(t))) {
				auto text = syntaxNode.GetText(t);
				switch (f.GetStyle().end_statement_with_semicolon) {
					case EndStmtWithSemicolon::Always:
						// TODO handle stray semicolons that are unnecessarily added!
						/*if (syntaxNode.GetTextRange(t).Length < 2)
							break;*/
						if (!EndsWithSemicolon(syntaxNode, t)) {
							AddSemicolon(syntaxNode, t);
						}
						break;
					case EndStmtWithSemicolon::Never:
						// is on same line as other statement -> needs to go on new line
						if (!IsFirstStmtOfLine(syntaxNode, t)) {
							InsertNewLineBeforeNode(syntaxNode, t);
						}
						if (EndsWithSemicolon(syntaxNode, t)) {
							RemoveSemicolon(syntaxNode, t);
						}
						break;
					case EndStmtWithSemicolon::SameLine:
						if (EndsWithSemicolon(syntaxNode, t)) {
							if (IsLastStmtOfLine(syntaxNode, t)) {
								RemoveSemicolon(syntaxNode, t);
							}
						}
						break;
					default:
						break;
				}
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
		case SemicolonStrategy::InsertNewLine:
			resolve.SetTokenStrategy(TokenStrategy::NewLineBeforeToken);
			break;
		default:
			break;
		}
	}
}

void SemicolonAnalyzer::AddSemicolon(LuaSyntaxNode n, const LuaSyntaxTree& t) {
	auto token = GetLastNonCommentToken(n, t);
	if (token.IsToken(t)) {
		_semicolon[token.GetIndex()] = SemicolonStrategy::Add;
	}
}

void SemicolonAnalyzer::InsertNewLineBeforeNode(LuaSyntaxNode n, const LuaSyntaxTree& t) {
	auto token = n.GetFirstToken(t); // line breaks are put in front of the statement itself by non-first statements 
	if (token.IsToken(t)) {
		_semicolon[token.GetIndex()] = SemicolonStrategy::InsertNewLine;
	}
}

void SemicolonAnalyzer::RemoveSemicolon(LuaSyntaxNode n, const LuaSyntaxTree& t) {
	auto token = GetLastNonCommentToken(n, t);
	if (token.IsToken(t)) {
		_semicolon[token.GetIndex()] = SemicolonStrategy::Remove;
	}
}

bool SemicolonAnalyzer::IsFirstStmtOfLine(LuaSyntaxNode n, const LuaSyntaxTree& t) {
	// check if current stmt starts on same line as the previous one ends
	auto startCurrent = n.GetStartLine(t);
	auto prev = n.GetPrevToken(t);
	if (!prev.IsNull(t)) {
		auto endPrev = prev.GetEndLine(t);
		return endPrev < startCurrent;
	}
	return true; // there's no previous token
}

bool SemicolonAnalyzer::IsLastStmtOfLine(LuaSyntaxNode n, const LuaSyntaxTree& t) {
	// check if next stmt starts on same line as the current one ends
	auto currentEnd = n.GetEndLine(t);
	auto nextToken = n.GetNextToken(t);
	if (!nextToken.IsNull(t)) {
		auto nextStart = nextToken.GetStartLine(t);
		return currentEnd != nextStart;
	}
	return true; // there's no next token
}

bool SemicolonAnalyzer::EndsWithSemicolon(LuaSyntaxNode n, const LuaSyntaxTree& t) {
	auto token = GetLastNonCommentToken(n, t);
	auto text = token.GetText(t);

	return text == ";";
}

LuaSyntaxNode SemicolonAnalyzer::GetLastNonCommentToken(LuaSyntaxNode n, const LuaSyntaxTree& t) {
	auto token = n.GetLastToken(t);
	switch (token.GetTokenKind(t)) {
		case TK_SHORT_COMMENT:
		case TK_LONG_COMMENT:
		case TK_SHEBANG: {
			return token.GetPrevToken(t);
		}
		default: {
			return token;
		}
	}
}
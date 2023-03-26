#include "CodeService/Format/Analyzer/FormatDocAnalyze.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "Util/StringUtil.h"
#include "LuaParser/Lexer/TextReader.h"

FormatDocAnalyze::FormatDocAnalyze() {

}

void FormatDocAnalyze::Analyze(FormatState &f, const LuaSyntaxTree &t) {
//    for (auto syntaxNode: t.GetSyntaxNodes()) {
//        switch (syntaxNode.GetTokenKind(t)) {
//            case TK_SHORT_COMMENT: {
//                if (syntaxNode.GetParent(t).GetSyntaxKind(t) == LuaSyntaxNodeKind::Block) {
//
//
//                    break;
//                }
//            }
//            default: {
//
//            }
//        }
//    }
}

void FormatDocAnalyze::ComplexAnalyze(FormatState &f, const LuaSyntaxTree &t) {
    FormatAnalyzer::ComplexAnalyze(f, t);
}

void
FormatDocAnalyze::Query(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {

}

bool IsWhiteSpaces(int c) {
    return c > 0 && std::isspace(c);
}

bool ActionStart(int c) {
    return c > 0 && std::isalpha(c);
}

bool ActionContinue(int c) {
    return c > 0 && (std::isalnum(c) || c == '-');
}

void FormatDocAnalyze::AnalyzeDocFormat(LuaSyntaxNode n, FormatState &f, const LuaSyntaxTree &t) {
    auto text = n.GetText(t);
    TextReader textReader(text);

    enum class ParseState {
        Init,
        TagFormat,
        Action,
        List
    } state = ParseState::Init;

    while (!textReader.IsEof()) {
        textReader.ResetBuffer();

        if (IsWhiteSpaces(textReader.GetCurrentChar())) {
            textReader.EatWhile(IsWhiteSpaces);
            continue;
        }

        switch (state) {
            case ParseState::Init: {
                if (textReader.GetCurrentChar() == '-') {
                    auto dashNum = textReader.EatWhen('-');
                    if (dashNum == 3) {
                        state = ParseState::TagFormat;
                        break;
                    }
                }
                return;
            }
            case ParseState::TagFormat: {
                if (textReader.GetCurrentChar() == '@') {
                    textReader.NextChar();
                    textReader.EatWhile([](int c) { return c > 0 && std::isalpha(c); });
                    if (textReader.GetSaveText() == "format") {
                        state = ParseState::Action;
                        break;
                    }
                }
                return;
            }
            case ParseState::Action: {
                if (ActionStart(textReader.GetCurrentChar())) {
                    textReader.EatWhile(ActionContinue);
                    auto action = textReader.GetSaveText();
                    if (action == "disable") {

                    } else if (action == "disable-next") {

                    } else if (action == "on") {
                        state = ParseState::List;
                        // todo
                        break;
                    } else if (action == "on-next") {
                        state = ParseState::List;
                        // todo

                        break;
                    }
                }

                return;
            }
            case ParseState::List : {
                return;
            }
        }
    }
}

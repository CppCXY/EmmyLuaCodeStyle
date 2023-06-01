#include "CodeFormatCore/Format/Analyzer/FormatDocAnalyze.h"
#include "CodeFormatCore/Format/FormatState.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "LuaParser/Lexer/TextReader.h"
#include "Util/StringUtil.h"

FormatDocAnalyze::FormatDocAnalyze() {
}

void FormatDocAnalyze::Analyze(FormatState &f, const LuaSyntaxTree &t) {
    for (auto syntaxNode: t.GetSyntaxNodes()) {
        switch (syntaxNode.GetTokenKind(t)) {
            case TK_SHORT_COMMENT: {
                if (syntaxNode.GetParent(t).GetSyntaxKind(t) == LuaSyntaxNodeKind::Block) {
                    AnalyzeDocFormat(syntaxNode, f, t);
                    break;
                }
            }
            default: {
                break;
            }
        }
    }
}

void FormatDocAnalyze::ComplexAnalyze(FormatState &f, const LuaSyntaxTree &t) {
}

void FormatDocAnalyze::Query(FormatState &f, LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {
    auto it = _ignores.find(syntaxNode.GetIndex());
    if (it == _ignores.end()) {
        return;
    }

    f.AddIgnore(it->second);
    resolve.SetOriginRange(it->second);
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
                        auto block = n.GetParent(t);
                        auto lastChild = block.GetLastToken(t);
                        AddIgnoreRange(IndexRange(n.GetIndex(), lastChild.GetIndex()), t);
                        break;
                    } else if (action == "disable-next") {
                        auto nextNode = n.GetNextSibling(t);
                        while (!nextNode.IsNull(t) && !detail::multi_match::StatementMatch(nextNode.GetSyntaxKind(t))) {
                            nextNode.ToNext(t);
                        }
                        if (nextNode.IsNode(t)) {
                            AddIgnoreRange(IndexRange(n.GetIndex(), nextNode.GetIndex()), t);
                        }

                        break;
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
            case ParseState::List: {
                return;
            }
        }
    }
}

void FormatDocAnalyze::AddIgnoreRange(const IndexRange &range, const LuaSyntaxTree &t) {
    auto startIndex = LuaSyntaxNode(range.StartIndex).GetFirstToken(t).GetIndex();
    auto endIndex = LuaSyntaxNode(range.EndIndex).GetLastToken(t).GetIndex();
    _ignores[startIndex] = IndexRange(startIndex, endIndex);
}

std::vector<IndexRange> FormatDocAnalyze::GetIgnores() const {
    std::vector<IndexRange> result;
    for (auto p: _ignores) {
        result.push_back(p.second);
    }
    return result;
}

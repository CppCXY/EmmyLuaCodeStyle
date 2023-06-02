#include "LuaCompile/Lua/SyntaxTree/LuaTreeBuilder.h"
#include "LuaCompile/Lua/DocLexer/LuaDocLexer.h"
#include "LuaCompile/Lua/SyntaxTree/LuaDocTreeBuilder.h"
#include <cassert>


LuaTreeBuilder::LuaTreeBuilder()
    : _tokenIndex(0) {
}

void LuaTreeBuilder::BuildTree(LuaSyntaxTree &t, LuaParser &p) {

    StartNode(LuaSyntaxNodeKind::File, t, p);

    auto &events = p.GetParseState().GetEvents();
    std::vector<LuaSyntaxNodeKind> parents;
    for (auto &e: events) {
        switch (e.Type) {
            case MarkEventType::NodeStart: {
                if (e.U.Start.Kind == LuaSyntaxNodeKind::None) {
                    continue;
                }

                parents.push_back(e.U.Start.Kind);
                auto parentPos = e.U.Start.Parent;
                while (parentPos > 0) {
                    if (events[parentPos].Type == MarkEventType::NodeStart) {
                        auto &pe = events[parentPos];
                        parents.push_back(pe.U.Start.Kind);
                        parentPos = pe.U.Start.Parent;
                        pe.U.Start.Kind = LuaSyntaxNodeKind::None;
                    } else {
                        break;
                    }
                }

                for (auto rit = parents.rbegin(); rit != parents.rend(); rit++) {
                    StartNode(*rit, t, p);
                }

                parents.clear();
                break;
            }
            case MarkEventType::EatToken: {
                EatTrivias(t, p);
                EatToken(t, p);
                break;
            }
            case MarkEventType::NodeEnd: {
                FinishNode(t, p);
                break;
            }
            default: {
                break;
            }
        }
    }

    FinishNode(t, p);

    //    if (!t._nodeOrTokens.empty()) {
    //        t._syntaxNodes.reserve(t._nodeOrTokens.size() - 1);
    //        for (std::size_t i = 0; i != t._nodeOrTokens.size() - 1; i++) {
    //            t._syntaxNodes.emplace_back(i + 1);
    //        }
    //    }
}

void LuaTreeBuilder::StartNode(LuaSyntaxNodeKind kind, LuaSyntaxTree &t, LuaParser &p) {
    if (!IsEatAllTrivia(kind, t)) {
        auto triviaCount = CalculateTriviaCount(t, p);
        if (triviaCount != 0) {
            auto edge = BindLeftComment(_tokenIndex + triviaCount, t, p);
            assert(edge <= triviaCount);
            EatTriviaByCount(triviaCount - edge, t, p);
            BuildNode(kind, t);
            EatTriviaByCount(edge, t, p);
            return;
        }
    }
    BuildNode(kind, t);
}

std::size_t LuaTreeBuilder::CalculateTriviaCount(LuaSyntaxTree &t, LuaParser &p) {
    auto &tokens = p.GetTokens();
    std::size_t count = 0;
    for (auto index = _tokenIndex; index < tokens.size(); index++) {
        switch (tokens[index].TokenType) {
            case LuaTokenKind::TK_WS:
            case LuaTokenKind::TK_SHORT_COMMENT:
            case LuaTokenKind::TK_LONG_COMMENT:
            case LuaTokenKind::TK_SHEBANG: {
                count++;
                break;
            }
            default: {
                return count;
            }
        }
    }
    return count;
}

std::size_t LuaTreeBuilder::BindLeftComment(std::size_t startPos, LuaSyntaxTree &t, LuaParser &p) {
    auto &tokens = p.GetTokens();
    if (startPos >= tokens.size() || startPos <= _tokenIndex) {
        return 0;
    }
    auto &file = t.GetSource();
    auto &lineIndex = file.GetLineIndex();

    std::size_t count = 0;
    std::size_t total = startPos - _tokenIndex;
    for (; count < total; count++) {
        auto index = startPos - count - 1;
        auto &token = tokens[index];
        switch (token.TokenType) {
            case LuaTokenKind::TK_SHORT_COMMENT:
            case LuaTokenKind::TK_LONG_COMMENT:
            case LuaTokenKind::TK_SHEBANG: {
                break;
            }
            case LuaTokenKind::TK_WS: {
                if (lineIndex.GetLine(token.Range.StartOffset) + 1 < lineIndex.GetLine(token.Range.GetEndOffset() + 1)) {
                    return count;
                }
                break;
            }
            default: {
                return count;
            }
        }
    }
    return count;
}

std::size_t LuaTreeBuilder::BindRightComment(LuaSyntaxNodeKind kind, LuaSyntaxTree &t, LuaParser &p) {
    switch (kind) {
        case LuaSyntaxNodeKind::LocalStatement:
        case LuaSyntaxNodeKind::AssignStatement: {
            auto &tokens = p.GetTokens();
            if (_tokenIndex > 0 && _tokenIndex < tokens.size()) {
                auto index = _tokenIndex;
                std::size_t wsCount = 0;
                switch (tokens[index].TokenType) {
                    case LuaTokenKind::TK_SHORT_COMMENT:
                    case LuaTokenKind::TK_LONG_COMMENT:
                    case LuaTokenKind::TK_SHEBANG: {
                        auto prevToken = tokens[index - 1];
                        auto &file = t.GetSource();
                        if (file.GetLineIndex().GetLine(prevToken.Range.GetEndOffset()) == file.GetLineIndex().GetLine(tokens[index].Range.StartOffset)) {
                            return 1 + wsCount;
                        }
                        break;
                    }
                    case LuaTokenKind::TK_WS: {
                        wsCount++;
                        break;
                    }
                    default: {
                        return 0;
                    }
                }
            }
            break;
        }
        case LuaSyntaxNodeKind::Body:
        case LuaSyntaxNodeKind::TableFieldList:
        case LuaSyntaxNodeKind::File: {
            return CalculateTriviaCount(t, p);
        }
        default: {
            return 0;
        }
    }
    return 0;
}

void LuaTreeBuilder::EatTriviaByCount(std::size_t count, LuaSyntaxTree &t, LuaParser &p) {
    if (count == 0) {
        return;
    }
    enum class ParseState {
        Init,
        Comment
    } state = ParseState::Init;


    auto &source = t.GetSource();
    auto &lineIndex = source.GetLineIndex();

    auto &tokens = p.GetTokens();

    std::vector<std::size_t> comments;
    for (std::size_t i = 0; i < count; i++) {
        auto index = _tokenIndex + i;
        auto &token = tokens[index];
        switch (state) {
            case ParseState::Init: {
                if (token.TokenType == LuaTokenKind::TK_WS) {
                    BuildToken(token, t);
                } else {
                    comments.push_back(index);
                    state = ParseState::Comment;
                }
                break;
            }
            case ParseState::Comment: {
                if (i + 1 >= count) {
                    if (token.TokenType == LuaTokenKind::TK_WS) {
                        BuildComments(comments, t, p);
                        BuildToken(token, t);
                    } else {
                        comments.push_back(index);
                        BuildComments(comments, t, p);
                    }
                    break;
                }

                if (token.TokenType == LuaTokenKind::TK_WS) {
                    if (lineIndex.GetLine(token.Range.StartOffset) + 1 >= lineIndex.GetLine(token.Range.GetEndOffset() + 1)) {
                        comments.push_back(index);
                    } else {
                        BuildComments(comments, t, p);
                        comments.clear();
                        BuildToken(token, t);
                        state = ParseState::Init;
                    }
                } else {
                    comments.push_back(index);
                }
                break;
            }
        }
    }
    _tokenIndex += count;
}

void LuaTreeBuilder::EatTrivias(LuaSyntaxTree &t, LuaParser &p) {
    auto count = CalculateTriviaCount(t, p);
    EatTriviaByCount(count, t, p);
}

void LuaTreeBuilder::EatToken(LuaSyntaxTree &t, LuaParser &p) {
    auto &token = p.GetTokens()[_tokenIndex];
    _tokenIndex++;
    BuildToken(token, t);
}

void LuaTreeBuilder::FinishNode(LuaSyntaxTree &t, LuaParser &p) {
    if (!_nodePosStack.empty()) {
        auto nodePos = _nodePosStack.top();
        auto &node = t._nodeOrTokens[nodePos];
        if (node.Type == NodeOrTokenType::Node) {
            auto edge = BindRightComment(node.Data.Node.Kind, t, p);
            EatTriviaByCount(edge, t, p);
        }

        _nodePosStack.pop();
    }
}

void LuaTreeBuilder::BuildNode(LuaSyntaxNodeKind kind, LuaSyntaxTree &t) {
    auto currentPos = t._nodeOrTokens.size();
    auto &currentNode = t._nodeOrTokens.emplace_back(kind);
    if (!_nodePosStack.empty()) {
        auto parentIndex = _nodePosStack.top();
        auto &parentNode = t._nodeOrTokens[parentIndex];
        if (parentNode.FirstChild == 0) {
            parentNode.FirstChild = currentPos;
            parentNode.LastChild = currentPos;
        } else {
            auto &lastNode = t._nodeOrTokens[parentNode.LastChild];
            lastNode.NextSibling = currentPos;
            currentNode.PrevSibling = parentNode.LastChild;
            parentNode.LastChild = currentPos;
        }
        currentNode.Parent = parentIndex;
    }

    _nodePosStack.push(currentPos);
}

void LuaTreeBuilder::BuildToken(LuaToken &token, LuaSyntaxTree &t) {
    auto currentNodePos = t._nodeOrTokens.size();
    auto currentTokenPos = t._tokens.size();

    t._tokens.emplace_back(token, currentNodePos);
    auto &currentToken = t._nodeOrTokens.emplace_back(currentTokenPos);
    if (!_nodePosStack.empty()) {
        auto parentIndex = _nodePosStack.top();
        auto &parentNode = t._nodeOrTokens[parentIndex];
        if (parentNode.FirstChild == 0) {
            parentNode.FirstChild = currentNodePos;
            parentNode.LastChild = currentNodePos;
        } else {
            auto &lastNode = t._nodeOrTokens[parentNode.LastChild];
            lastNode.NextSibling = currentNodePos;
            currentToken.PrevSibling = parentNode.LastChild;
            parentNode.LastChild = currentNodePos;
        }
        currentToken.Parent = parentIndex;
    }
}

bool LuaTreeBuilder::IsEatAllTrivia(LuaSyntaxNodeKind kind, LuaSyntaxTree &t) const {
    return kind == LuaSyntaxNodeKind::Body || kind == LuaSyntaxNodeKind::TableFieldList || kind == LuaSyntaxNodeKind::File;
}

void LuaTreeBuilder::BuildComments(std::vector<std::size_t> &group, LuaSyntaxTree &t, LuaParser &p) {
    if (_nodePosStack.empty()) {
        return;
    }

    const auto &file = t.GetSource();
    auto &tokens = p.GetTokens();
    std::vector<LuaToken> luaDocTokens;
    for (auto i: group) {
        auto &luaToken = tokens[i];
        LuaDocLexer docLexer(file.Slice(luaToken.Range), luaToken.Range.StartOffset);
        for (auto &docToken: docLexer.Tokenize()) {
            luaDocTokens.emplace_back(docToken);
        }
    }

    LuaDocParser docParser(&file, std::move(luaDocTokens));
    docParser.Parse();
    LuaDocTreeBuilder docTreeBuilder;

    docTreeBuilder.BuildTree(t, docParser, _nodePosStack.top());
}

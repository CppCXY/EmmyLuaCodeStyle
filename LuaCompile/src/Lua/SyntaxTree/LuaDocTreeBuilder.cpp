#include "LuaCompile/Lua/SyntaxTree/LuaDocTreeBuilder.h"

using enum LuaTokenKind;

LuaDocTreeBuilder::LuaDocTreeBuilder()
    : _tokenIndex(0) {
}

void LuaDocTreeBuilder::BuildTree(LuaSyntaxTree &t, LuaDocParser &p, std::size_t parent) {
    _nodePosStack.push(parent);

    StartNode(LuaSyntaxNodeKind::Comment, t, p);

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
}

void LuaDocTreeBuilder::StartNode(LuaSyntaxNodeKind kind, LuaSyntaxTree &t, LuaDocParser &p) {
    BuildNode(kind, t);
}

void LuaDocTreeBuilder::EatToken(LuaSyntaxTree &t, LuaDocParser &p) {
    auto &token = p.GetTokens()[_tokenIndex];
    _tokenIndex++;
    BuildToken(token, t);
}

void LuaDocTreeBuilder::FinishNode(LuaSyntaxTree &t, LuaDocParser &p) {
    if (!_nodePosStack.empty()) {
//        auto nodePos = _nodePosStack.top();
        _nodePosStack.pop();
    }
}

void LuaDocTreeBuilder::BuildNode(LuaSyntaxNodeKind kind, LuaSyntaxTree &t) {
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

void LuaDocTreeBuilder::BuildToken(LuaToken &token, LuaSyntaxTree &t) {
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

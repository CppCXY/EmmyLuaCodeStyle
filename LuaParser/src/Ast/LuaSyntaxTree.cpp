#include "LuaParser/Ast/LuaSyntaxTree.h"
#include <algorithm>
#include "LuaParser/Parse/LuaParser.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "Util/format.h"

LuaSyntaxTree::LuaSyntaxTree()
        : _file(),
          _tokenIndex(0) {

}

void LuaSyntaxTree::BuildTree(LuaParser &p) {
    _file = p.GetLuaFile();
    StartNode(LuaSyntaxNodeKind::File, p);

    auto &events = p.GetEvents();
    std::vector<LuaSyntaxNodeKind> parents;
    for (auto i = 0ull; i != events.size(); i++) {
        switch (events[i].Type) {
            case MarkEventType::NodeStart: {
                auto e = events[i];
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
                for (auto rIt = parents.rbegin(); rIt != parents.rend(); rIt++) {
                    StartNode(*rIt, p);
                }
                parents.clear();
                break;
            }
            case MarkEventType::EatToken: {
                EatComments(p);
                EatToken(p);
                break;
            }
            case MarkEventType::NodeEnd: {
                FinishNode(p);
                break;
            }
        }
    }

    FinishNode(p);

    _syntaxNodes.reserve(_nodeOrTokens.size() - 1);
    for (auto i = 0; i != _nodeOrTokens.size() - 1; i++) {
        _syntaxNodes.emplace_back(i + 1);
    }
}

void LuaSyntaxTree::StartNode(LuaSyntaxNodeKind kind, LuaParser &p) {
    if (kind != LuaSyntaxNodeKind::Block) {
        EatComments(p);
        BuildNode(kind);
    } else {
        BuildNode(kind);
    }
}

void LuaSyntaxTree::EatComments(LuaParser &p) {
    auto &tokens = p.GetTokens();
    for (; _tokenIndex < tokens.size(); _tokenIndex++) {
        switch (tokens[_tokenIndex].TokenType) {
            case TK_SHORT_COMMENT:
            case TK_LONG_COMMENT:
            case TK_SHEBANG: {
                EatToken(p);
                break;
            }
            default: {
                return;
            }
        }
    }
}

void LuaSyntaxTree::EatToken(LuaParser &p) {
    auto &token = p.GetTokens()[_tokenIndex];
    _tokenIndex++;
    BuildToken(token);
}

void LuaSyntaxTree::FinishNode(LuaParser &p) {
    if (!_nodePosStack.empty()) {
        auto nodePos = _nodePosStack.top();
        auto &node = _nodeOrTokens[nodePos];
        if (node.Type == NodeOrTokenType::Node && node.Data.NodeKind == LuaSyntaxNodeKind::Block) {
            EatComments(p);
        } else {
            if (_tokenIndex < p.GetTokens().size() && _tokenIndex > 0) {
                auto nextToken = p.GetTokens()[_tokenIndex];
                switch (nextToken.TokenType) {
                    case TK_SHORT_COMMENT:
                    case TK_LONG_COMMENT: {
                        auto nextLine = _file->GetLine(nextToken.Range.StartOffset);
                        auto currentLine = _file->GetLine(p.GetTokens()[_tokenIndex - 1].Range.EndOffset);
                        // inline comment
                        if (currentLine == nextLine) {
                            EatToken(p);
                        }
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
        }

        _nodePosStack.pop();
    }
}

void LuaSyntaxTree::BuildNode(LuaSyntaxNodeKind kind) {
    auto currentPos = _nodeOrTokens.size();
    auto &currentNode = _nodeOrTokens.emplace_back(kind);
    if (!_nodePosStack.empty()) {
        auto parentIndex = _nodePosStack.top();
        auto &parentNode = _nodeOrTokens[parentIndex];
        if (parentNode.FirstChild == 0) {
            parentNode.FirstChild = currentPos;
            parentNode.LastChild = currentPos;
        } else {
            auto &lastNode = _nodeOrTokens[parentNode.LastChild];
            lastNode.NextSibling = currentPos;
            currentNode.PrevSibling = parentNode.LastChild;
            parentNode.LastChild = currentPos;
        }
    }
    _nodePosStack.push(currentPos);
}

void LuaSyntaxTree::BuildToken(LuaToken &token) {
    auto currentPos = _nodeOrTokens.size();
    auto &currentNode = _nodeOrTokens.emplace_back(token);
    if (!_nodePosStack.empty()) {
        auto parentIndex = _nodePosStack.top();
        auto &parentNode = _nodeOrTokens[parentIndex];
        if (parentNode.FirstChild == 0) {
            parentNode.FirstChild = currentPos;
            parentNode.LastChild = currentPos;
        } else {
            auto &lastNode = _nodeOrTokens[parentNode.LastChild];
            lastNode.NextSibling = currentPos;
            currentNode.PrevSibling = parentNode.LastChild;
            parentNode.LastChild = currentPos;
        }
        currentNode.Parent = parentIndex;
    }
}

const LuaFile &LuaSyntaxTree::GetFile() const {
    return *_file;
}

std::size_t LuaSyntaxTree::GetStartOffset(std::size_t index) const {
    if (index < _nodeOrTokens.size()) {
        auto &n = _nodeOrTokens[index];
        if (n.Type == NodeOrTokenType::Node) {
            auto child = n.FirstChild;
            while (IsNode(child)) {
                child = GetFirstChild(child);
            }
            return _nodeOrTokens[child].Data.Token.Start;
        } else {
            return n.Data.Token.Start;
        }
    }
    return 0;
}

std::size_t LuaSyntaxTree::GetEndOffset(std::size_t index) const {
    if (index < _nodeOrTokens.size()) {
        auto &n = _nodeOrTokens[index];
        if (n.Type == NodeOrTokenType::Node) {
            auto child = n.LastChild;
            while (IsNode(child)) {
                child = GetLastChild(child);
            }
            return _nodeOrTokens[child].Data.Token.Start + _nodeOrTokens[child].Data.Token.Length - 1;
        } else {
            return n.Data.Token.Start + n.Data.Token.Length - 1;
        }
    }
    return 0;
}

std::size_t LuaSyntaxTree::GetNextSibling(std::size_t index) const {
    if (index < _nodeOrTokens.size()) {
        return _nodeOrTokens[index].NextSibling;
    }
    return 0;
}

std::size_t LuaSyntaxTree::GetPrevSibling(std::size_t index) const {
    if (index < _nodeOrTokens.size()) {
        return _nodeOrTokens[index].PrevSibling;
    }
    return 0;
}

std::size_t LuaSyntaxTree::GetFirstChild(std::size_t index) const {
    if (index < _nodeOrTokens.size()) {
        return _nodeOrTokens[index].FirstChild;
    }
    return 0;
}

std::size_t LuaSyntaxTree::GetLastChild(std::size_t index) const {
    if (index < _nodeOrTokens.size()) {
        return _nodeOrTokens[index].LastChild;
    }
    return 0;
}

std::size_t LuaSyntaxTree::GetFirstToken(std::size_t index) const {
    if (index < _nodeOrTokens.size()) {
        auto &n = _nodeOrTokens[index];
        if (n.Type == NodeOrTokenType::Node) {
            auto child = n.FirstChild;
            while (IsNode(child)) {
                child = GetFirstChild(child);
            }
            return child;
        } else {
            return index;
        }
    }
    return 0;
}

std::size_t LuaSyntaxTree::GetParent(std::size_t index) const {
    if (index < _nodeOrTokens.size()) {
        return _nodeOrTokens[index].Parent;
    }
    return 0;
}

LuaSyntaxNodeKind LuaSyntaxTree::GetNodeKind(std::size_t index) const {
    if (!IsNode(index)) {
        return LuaSyntaxNodeKind::None;
    }
    return _nodeOrTokens[index].Data.NodeKind;
}

LuaTokenKind LuaSyntaxTree::GetTokenKind(std::size_t index) const {
    if (!IsToken(index)) {
        return LuaTokenKind(0);
    }
    return _nodeOrTokens[index].Data.Token.Kind;
}

bool LuaSyntaxTree::IsNode(std::size_t index) const {
    if (index == 0 || (_nodeOrTokens.size() <= index)) {
        return false;
    }
    return _nodeOrTokens[index].Type == NodeOrTokenType::Node;
}

bool LuaSyntaxTree::IsToken(std::size_t index) const {
    if (index == 0 || (_nodeOrTokens.size() <= index)) {
        return false;
    }
    return _nodeOrTokens[index].Type == NodeOrTokenType::Token;
}

const std::vector<LuaSyntaxNode> &LuaSyntaxTree::GetSyntaxNodes() const {
    return _syntaxNodes;
}

std::vector<LuaSyntaxNode> LuaSyntaxTree::GetTokens() const {
    std::vector<LuaSyntaxNode> results;
    if (_nodeOrTokens.empty()) {
        return results;
    }

    results.reserve(_nodeOrTokens.size() - 1);
    for (auto i = 0; i != _nodeOrTokens.size() - 1; i++) {
        LuaSyntaxNode syntaxNode(i + 1);
        if (syntaxNode.IsToken(*this)) {
            results.push_back(syntaxNode);
        }
    }

    std::sort(results.begin(), results.end(), [this](LuaSyntaxNode &l, LuaSyntaxNode &r) {
        return GetStartOffset(l.GetIndex()) < GetStartOffset(r.GetIndex());
    });

    return results;
}

LuaSyntaxNode LuaSyntaxTree::GetRootNode() const {
    return LuaSyntaxNode(1);
}

std::string LuaSyntaxTree::GetDebugView() {
    std::string debugView;
    debugView.append("{ Lua Syntax Tree, filename }\n");

    auto root = GetRootNode();
    std::stack<LuaSyntaxNode> traverseStack;
    std::size_t indent = 1;
    traverseStack.push(root);
    // 非递归深度优先遍历
    while (!traverseStack.empty()) {
        LuaSyntaxNode node = traverseStack.top();
        if (node.IsNode(*this)) {
            traverseStack.top() = LuaSyntaxNode(0);
            auto children = node.GetChildren(*this);
            for (auto rIt = children.rbegin(); rIt != children.rend(); rIt++) {
                traverseStack.push(*rIt);
            }
            debugView.resize(debugView.size() + indent, '\t');
            debugView.append(util::format("{{ Lua Syntax Node, index: {} ,Kind: {} }}\n",
                                          node.GetIndex(),
                                          detail::debug::GetSyntaxKindDebugName(node.GetSyntaxKind(*this))));
            indent++;
        } else if (node.IsToken(*this)) {
            traverseStack.pop();
            debugView.resize(debugView.size() + indent, '\t');
            debugView.append(util::format("{{ Lua Syntax Token, index: {} , Token: {} }}\n",
                                          node.GetIndex(),
                                          node.GetText(*this)));
        } else {
            traverseStack.pop();
            indent--;
        }
    }
    return debugView;
}

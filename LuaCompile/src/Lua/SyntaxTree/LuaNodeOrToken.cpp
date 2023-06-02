#include "LuaCompile/Lua/SyntaxTree/LuaNodeOrToken.h"

LuaNodeOrToken::LuaNodeOrToken(std::size_t index)
    : _index(index) {
}

TextRange LuaNodeOrToken::GetTextRange(const LuaSyntaxTree &t) const {
    if (IsToken(t)) {
        return t.GetTokenRange(_index);
    }
    if (IsNull(t)) {
        return TextRange(0, 0);
    }

    auto start = t.GetStartOffset(_index);
    auto end = t.GetEndOffset(_index);
    if (end >= start) {
        return TextRange(start, end - start + 1);
    }
    return TextRange(start, 0);
}

std::size_t LuaNodeOrToken::GetStartLine(const LuaSyntaxTree &t) const {
    return t.GetSource().GetLineIndex().GetLine(t.GetStartOffset(_index));
}

std::size_t LuaNodeOrToken::GetStartCol(const LuaSyntaxTree &t) const {
    return t.GetSource().GetLineIndex().GetCol(t.GetStartOffset(_index));
}

std::size_t LuaNodeOrToken::GetEndLine(const LuaSyntaxTree &t) const {
    return t.GetSource().GetLineIndex().GetLine(t.GetEndOffset(_index));
}

std::size_t LuaNodeOrToken::GetEndCol(const LuaSyntaxTree &t) const {
    return t.GetSource().GetLineIndex().GetCol(t.GetEndOffset(_index));
}

std::string_view LuaNodeOrToken::GetText(const LuaSyntaxTree &t) const {
    return t.GetSource().Slice(GetTextRange(t));
}

bool LuaNodeOrToken::IsNode(const LuaSyntaxTree &t) const {
    return t.IsNode(_index);
}

LuaSyntaxNodeKind LuaNodeOrToken::GetSyntaxKind(const LuaSyntaxTree &t) const {
    return t.GetNodeKind(_index);
}

LuaTokenKind LuaNodeOrToken::GetTokenKind(const LuaSyntaxTree &t) const {
    return t.GetTokenKind(_index);
}

LuaNodeOrToken LuaNodeOrToken::GetParent(const LuaSyntaxTree &t) const {
    return LuaNodeOrToken(t.GetParent(_index));
}

LuaNodeOrToken LuaNodeOrToken::GetNextSibling(const LuaSyntaxTree &t) const {
    return LuaNodeOrToken(t.GetNextSibling(_index));
}

LuaNodeOrToken LuaNodeOrToken::GetPrevSibling(const LuaSyntaxTree &t) const {
    return LuaNodeOrToken(t.GetPrevSibling(_index));
}

LuaNodeOrToken LuaNodeOrToken::GetFirstChild(const LuaSyntaxTree &t) const {
    return LuaNodeOrToken(t.GetFirstChild(_index));
}

LuaNodeOrToken LuaNodeOrToken::GetLastChild(const LuaSyntaxTree &t) const {
    return LuaNodeOrToken(t.GetLastChild(_index));
}

void LuaNodeOrToken::ToNext(const LuaSyntaxTree &t) {
    _index = t.GetNextSibling(_index);
}

void LuaNodeOrToken::ToPrev(const LuaSyntaxTree &t) {
    _index = t.GetPrevSibling(_index);
}

bool LuaNodeOrToken::IsNull(const LuaSyntaxTree &t) const {
    return _index == 0;
}

std::vector<LuaNodeOrToken> LuaNodeOrToken::GetDescendants(const LuaSyntaxTree &t) const {
    std::vector<LuaNodeOrToken> results;
    if (t.GetFirstChild(_index) == 0) {
        return results;
    }

    int accessIndex = -1;
    LuaNodeOrToken node = *this;
    do {
        if (node.IsNode(t)) {
            for (auto child = node.GetFirstChild(t); !child.IsNull(t); child.ToNext(t)) {
                results.emplace_back(child);
            }
        }
        accessIndex++;
    } while (accessIndex < static_cast<int>(results.size()));
    return results;
}

std::vector<LuaNodeOrToken> LuaNodeOrToken::GetChildren(const LuaSyntaxTree &t) const {
    std::vector<LuaNodeOrToken> results;
    for (auto child = GetFirstChild(t); !child.IsNull(t); child.ToNext(t)) {
        results.push_back(child);
    }
    return results;
}

bool LuaNodeOrToken::IsToken(const LuaSyntaxTree &t) const {
    return t.IsToken(_index);
}

std::size_t LuaNodeOrToken::GetIndex() const {
    return _index;
}

LuaNodeOrToken LuaNodeOrToken::GetChildSyntaxNode(LuaSyntaxNodeKind kind, const LuaSyntaxTree &t) const {
    auto child = GetFirstChild(t);
    for (; !child.IsNull(t); child.ToNext(t)) {
        if (child.GetSyntaxKind(t) == kind) {
            break;
        }
    }
    return child;
}

LuaNodeOrToken LuaNodeOrToken::GetChildSyntaxNode(LuaSyntaxMultiKind kind, const LuaSyntaxTree &t) const {
    auto child = GetFirstChild(t);
    for (; !child.IsNull(t); child.ToNext(t)) {
        if (detail::multi_match::Match(kind, child.GetSyntaxKind(t))) {
            break;
        }
    }
    return child;
}

LuaNodeOrToken LuaNodeOrToken::GetChildToken(LuaTokenKind kind, const LuaSyntaxTree &t) const {
    auto child = GetFirstChild(t);
    for (; !child.IsNull(t); child.ToNext(t)) {
        if (child.GetTokenKind(t) == kind) {
            break;
        }
    }
    return child;
}

LuaNodeOrToken LuaNodeOrToken::GetChildToken(std::function<bool(LuaTokenKind)> predicated, const LuaSyntaxTree &t) const {
    auto child = GetFirstChild(t);
    for (; !child.IsNull(t); child.ToNext(t)) {
        if (predicated(child.GetTokenKind(t))) {
            break;
        }
    }
    return child;
}

std::vector<LuaNodeOrToken> LuaNodeOrToken::GetChildSyntaxNodes(LuaSyntaxNodeKind kind, const LuaSyntaxTree &t) const {
    std::vector<LuaNodeOrToken> results;
    auto child = GetFirstChild(t);
    for (; !child.IsNull(t); child.ToNext(t)) {
        if (child.GetSyntaxKind(t) == kind) {
            results.push_back(child);
        }
    }
    return results;
}

std::vector<LuaNodeOrToken> LuaNodeOrToken::GetChildSyntaxNodes(LuaSyntaxMultiKind kind, const LuaSyntaxTree &t) const {
    std::vector<LuaNodeOrToken> results;
    auto child = GetFirstChild(t);
    for (; !child.IsNull(t); child.ToNext(t)) {
        if (detail::multi_match::Match(kind, child.GetSyntaxKind(t))) {
            results.push_back(child);
        }
    }
    return results;
}

std::vector<LuaNodeOrToken> LuaNodeOrToken::GetChildTokens(LuaTokenKind kind, const LuaSyntaxTree &t) const {
    std::vector<LuaNodeOrToken> results;
    auto child = GetFirstChild(t);
    for (; !child.IsNull(t); child.ToNext(t)) {
        if (child.GetTokenKind(t) == kind) {
            results.push_back(child);
        }
    }
    return results;
}

LuaNodeOrToken LuaNodeOrToken::GetNextToken(const LuaSyntaxTree &t) const {
    if (_index == 0) {
        return *this;
    }
    return LuaNodeOrToken(t.GetNextToken(_index));
}

LuaNodeOrToken LuaNodeOrToken::GetPrevToken(const LuaSyntaxTree &t) const {
    if (_index == 0) {
        return *this;
    }
    return LuaNodeOrToken(t.GetPrevToken(_index));
}

LuaNodeOrToken LuaNodeOrToken::GetFirstToken(const LuaSyntaxTree &t) const {
    if (_index == 0) {
        return *this;
    }
    return LuaNodeOrToken(t.GetFirstToken(_index));
}

LuaNodeOrToken LuaNodeOrToken::GetLastToken(const LuaSyntaxTree &t) const {
    if (_index == 0) {
        return *this;
    }
    return LuaNodeOrToken(t.GetLastToken(_index));
}

std::size_t LuaNodeOrToken::GetFirstLineWidth(const LuaSyntaxTree &t) const {
    auto text = GetText(t);
    return utf8::Utf8nLenAtFirstLine(text.data(), text.size());
}

std::size_t LuaNodeOrToken::CountTokenChild(LuaTokenKind kind, const LuaSyntaxTree &t) const {
    auto count = 0;
    for (auto child = GetFirstChild(t); !child.IsNull(t); child.ToNext(t)) {
        if (child.GetTokenKind(t) == kind) {
            count++;
        }
    }
    return count;
}

std::size_t LuaNodeOrToken::CountNodeChild(LuaSyntaxNodeKind kind, const LuaSyntaxTree &t) const {
    auto count = 0;
    for (auto child = GetFirstChild(t); !child.IsNull(t); child.ToNext(t)) {
        if (child.GetSyntaxKind(t) == kind) {
            count++;
        }
    }
    return count;
}

bool LuaNodeOrToken::IsEmpty(const LuaSyntaxTree &t) const {
    return t.GetFirstChild(_index) == 0;
}

LuaNodeOrToken LuaNodeOrToken::Ancestor(const LuaSyntaxTree &t, const std::function<bool(LuaSyntaxNodeKind, bool &)> &predicate) const {
    LuaNodeOrToken parent = GetParent(t);
    bool ct = true;
    while (parent.IsNode(t) && ct) {
        if (predicate(parent.GetSyntaxKind(t), ct)) {
            return parent;
        }
    }
    return LuaNodeOrToken();
}

LuaNodeOrToken LuaNodeOrToken::FindChild(const LuaSyntaxTree &t, const std::function<bool(LuaSyntaxNodeKind)> &predicate) const {
    for (auto child = GetFirstChild(t); !child.IsNull(t); child.ToNext(t)) {
        if (predicate(child.GetSyntaxKind(t))) {
            return child;
        }
    }
    return LuaNodeOrToken();
}

std::vector<LuaNodeOrToken> LuaNodeOrToken::FindChildren(const LuaSyntaxTree &t, const std::function<bool(LuaSyntaxNodeKind)> &predicate) const {
    std::vector<LuaNodeOrToken> results;
    for (auto child = GetFirstChild(t); !child.IsNull(t); child.ToNext(t)) {
        if (predicate(child.GetSyntaxKind(t))) {
            results.emplace_back(child.GetIndex());
        }
    }
    return results;
}

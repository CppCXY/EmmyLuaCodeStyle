#include "SyntaxNodeIterator.h"

SyntaxNodeIterator::SyntaxNodeIterator(LuaNodeOrToken n) {
}

std::vector<LuaNodeOrToken> SyntaxNodeIterator::Collect(LuaTokenKind kind, const LuaSyntaxTree &t) const {
    std::vector<LuaNodeOrToken> result;
    for (auto start = _n; !start.IsNull(t); start.ToNext(t)) {
        if (start.GetTokenKind(t) == kind) {
            result.push_back(start);
        }
    }
    return result;
}

std::vector<LuaNodeOrToken> SyntaxNodeIterator::Collect(LuaSyntaxNodeKind kind, const LuaSyntaxTree &t) const {
    std::vector<LuaNodeOrToken> result;
    for (auto start = _n; !start.IsNull(t); start.ToNext(t)) {
        if (start.GetSyntaxKind(t) == kind) {
            result.push_back(start);
        }
    }
    return result;
}

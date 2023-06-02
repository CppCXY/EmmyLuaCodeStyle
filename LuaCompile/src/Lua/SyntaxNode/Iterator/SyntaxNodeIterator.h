#pragma once

#include <vector>

#include "Lua/SyntaxTree/LuaNodeOrToken.h"

// TODO 改成惰性求值
class SyntaxNodeIterator {
public:
    explicit SyntaxNodeIterator(LuaNodeOrToken n);

    template<class Syntax>
    std::vector<Syntax> Collect(const LuaSyntaxTree &t) const {
        std::vector<Syntax> result;
        for (auto start = _n; !start.IsNull(t); start.ToNext(t)) {
            if (Syntax::CanCast(start.GetSyntaxKind(t))) {
                result.push_back(Syntax(start));
            }
        }
        return result;
    }

    std::vector<LuaNodeOrToken> Collect(LuaTokenKind kind, const LuaSyntaxTree &t) const;

    std::vector<LuaNodeOrToken> Collect(LuaSyntaxNodeKind kind, const LuaSyntaxTree &t) const;


private:
    LuaNodeOrToken _n;
};

#pragma once

#include <string_view>
#include <vector>
#include <memory>
#include "LuaAstNodeType.h"
#include "LuaParser/Types/TextRange.h"
#include "LuaParser/Lexer/LuaTokenType.h"

class LuaAstTree;

class LuaAstNode
{
public:
	explicit LuaAstNode(std::size_t index);

	TextRange GetTextRange(const LuaAstTree& t) const;

    std::size_t GetStartLine(const LuaAstTree& t) const;

    std::size_t GetStartCol(const LuaAstTree& t) const;

    std::size_t GetEndLine(const LuaAstTree& t) const;

    std::size_t GetEndCol(const LuaAstTree& t) const;

	std::string_view GetText(const LuaAstTree& t) const;

    bool IsNode(const LuaAstTree& t) const;

	LuaAstNodeType GetType(const LuaAstTree& t) const;

    LuaTokenType GetTokenType(const LuaAstTree& t) const;

	LuaAstNode GetParent(const LuaAstTree& t) const;

    LuaAstNode GetSibling(const LuaAstTree& t) const;

    LuaAstNode GetFirstChild(const LuaAstTree& t) const;

    void ToNext(const LuaAstTree& t);

    bool IsNull(const LuaAstTree& t) const;

private:
	std::size_t _index;
};


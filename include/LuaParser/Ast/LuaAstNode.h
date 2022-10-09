#pragma once

#include <string_view>
#include <vector>
#include <memory>
#include "LuaAstNodeType.h"

class LuaAstTree;

class LuaAstNode
{
public:
	LuaAstNode(std::size_t index, std::shared_ptr<LuaAstTree> tree);

	TextRange GetTextRange() const;

	std::string_view GetText() const;

	LuaAstNodeType GetType() const;

	LuaTokenType GetTokenType() const;

	LuaAstNode& GetParent();

	void AddChild(LuaAstNode& child);

protected:
	std::size_t _index;
	std::weak_ptr<LuaAstTree> _tree;
};


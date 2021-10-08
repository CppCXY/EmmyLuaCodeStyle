#pragma once

#include <string_view>
#include <vector>
#include <memory>
#include "LuaAstNodeType.h"
#include "TextRange.h"
#include "LuaToken.h"

class LuaAstNode : std::enable_shared_from_this<LuaAstNode>
{
public:
	LuaAstNode(LuaAstNodeType type, const char* source);

	LuaAstNode(LuaAstNodeType type, LuaToken& token);

	TextRange GetTextRange() const;

	std::string_view GetText() const;

	const std::vector<std::shared_ptr<LuaAstNode>>& GetChildren() const;

	void AddChild(std::shared_ptr<LuaAstNode> child);

	LuaAstNodeType GetType() const;


private:
	LuaAstNodeType _type;
	std::weak_ptr<LuaAstNode> _parent;

	const char* _source;
	TextRange _textRange;
	std::vector<std::shared_ptr<LuaAstNode>> _children;

};

#pragma once

#include <string_view>
#include <vector>
#include <memory>
#include "LuaAstNodeType.h"
#include "TextRange.h"
#include "LuaToken.h"

class LuaAstNode
{
public:
	LuaAstNode(LuaAstNodeType type, const char* source);

	LuaAstNode(LuaAstNodeType type, LuaToken& token);

	std::shared_ptr<LuaAstNode> Copy();

	TextRange GetTextRange() const;

	std::string_view GetText() const;

	const std::vector<std::shared_ptr<LuaAstNode>>& GetChildren();

	void AddChild(std::shared_ptr<LuaAstNode> child);

	LuaAstNodeType GetType() const;

	void SetType(LuaAstNodeType type);

	// child会被加入为最底层的叶子节点
	void AddLeafChild(std::shared_ptr<LuaAstNode> child);
private:
	void addChildAfter(int index, std::shared_ptr<LuaAstNode> child);
	void addChildBefore(int index, std::shared_ptr<LuaAstNode> child);

	LuaAstNodeType _type;
	std::string_view _text;

	const char* _source;
	TextRange _textRange;
	std::vector<std::shared_ptr<LuaAstNode>> _children;
	std::string _error;
};

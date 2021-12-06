#pragma once

#include <string_view>
#include <vector>
#include <memory>
#include "LuaAstNodeType.h"
#include "TextRange.h"
#include "LuaToken.h"

class LuaAstVisitor;

class LuaAstNode: public std::enable_shared_from_this<LuaAstNode>
{
public:
	LuaAstNode(LuaAstNodeType type, const char* source);

	LuaAstNode(LuaAstNodeType type, LuaToken& token);

	~LuaAstNode();

	std::shared_ptr<LuaAstNode> Copy();

	std::shared_ptr<LuaAstNode> FindFirstOf(LuaAstNodeType type);

	void AcceptChildren(LuaAstVisitor& visitor);

	void Accept(LuaAstVisitor& visitor);

	TextRange GetTextRange() const;

	std::string_view GetText() const;

	const std::vector<std::shared_ptr<LuaAstNode>>& GetChildren();

	void AddChild(std::shared_ptr<LuaAstNode> child);

	LuaAstNodeType GetType() const;

	void SetType(LuaAstNodeType type);

	// child会被加入为最底层的叶子节点
	void AddLeafChild(std::shared_ptr<LuaAstNode> child);

	std::shared_ptr<LuaAstNode> GetParent();
private:
	void addChildAfter(int index, std::shared_ptr<LuaAstNode> child);
	void addChildBefore(int index, std::shared_ptr<LuaAstNode> child);

	LuaAstNodeType _type;
	std::string_view _text;

	const char* _source;
	TextRange _textRange;
	std::vector<std::shared_ptr<LuaAstNode>> _children;
	std::string _error;
	std::weak_ptr<LuaAstNode> _parent;
};

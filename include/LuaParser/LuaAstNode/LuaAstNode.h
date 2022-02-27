﻿#pragma once

#include <string_view>
#include <vector>
#include <memory>
#include "LuaAstNodeType.h"
#include "LuaParser/TextRange.h"
#include "LuaParser/LuaToken.h"

class LuaAstVisitor;

class LuaAstNode: public std::enable_shared_from_this<LuaAstNode>
{
public:

	using ChildrenContainer = std::vector<std::shared_ptr<LuaAstNode>>;
	using ChildIterator = ChildrenContainer::iterator;

	LuaAstNode(LuaAstNodeType type, std::string_view text, TextRange range);

	LuaAstNode(LuaAstNodeType type, LuaToken& token);

	~LuaAstNode();

	std::shared_ptr<LuaAstNode> Copy();

	std::shared_ptr<LuaAstNode> FindFirstOf(LuaAstNodeType type);

	void AcceptChildren(LuaAstVisitor& visitor);

	void Accept(LuaAstVisitor& visitor);

	TextRange GetTextRange() const;

	std::string_view GetText() const;

	ChildrenContainer& GetChildren();

	void AddChild(std::shared_ptr<LuaAstNode> child);

	LuaAstNodeType GetType() const;

	virtual void AddComment(std::shared_ptr<LuaAstNode> comment);

	std::shared_ptr<LuaAstNode> GetParent();

	const char* GetSource();

	bool Is(LuaAstNodeType type);

protected:
	void AddChildBefore(ChildIterator it, std::shared_ptr<LuaAstNode> child);

	LuaAstNodeType _type;
	std::string_view _text;
	TextRange _textRange;

	ChildrenContainer _children;
	std::weak_ptr<LuaAstNode> _parent;
};


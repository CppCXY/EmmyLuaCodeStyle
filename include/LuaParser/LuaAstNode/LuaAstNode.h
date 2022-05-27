#pragma once

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
	LUA_AST(LuaAstNode);

	using ChildrenContainer = std::vector<std::shared_ptr<LuaAstNode>>;
	using ChildIterator = ChildrenContainer::iterator;

	LuaAstNode(LuaAstNodeType type, std::string_view text, TextRange range, LuaTokenType tokenType = 0);

	LuaAstNode(LuaAstNodeType type, LuaToken& token);

	virtual ~LuaAstNode();

	std::shared_ptr<LuaAstNode> Copy();

	std::shared_ptr<LuaAstNode> FindFirstOf(LuaAstNodeType type);

	void AcceptChildren(LuaAstVisitor& visitor);

	void Accept(LuaAstVisitor& visitor);

	TextRange GetTextRange() const;

	std::string_view GetText() const;

	ChildrenContainer& GetChildren();

	void AddChild(std::shared_ptr<LuaAstNode> child);

	LuaAstNodeType GetType() const;

	LuaTokenType GetTokenType() const;

	virtual void AddComment(std::shared_ptr<LuaAstNode> comment);

	std::shared_ptr<LuaAstNode> GetParent();

	const char* GetSource();

	// luaParser 保持在C++17
	template<class T>
	bool Is()
	{
		return _type == T::TypeIndex;
	}

	template<class T>
	std::shared_ptr<T> As()
	{
		return std::dynamic_pointer_cast<T>(shared_from_this());
	}

protected:
	void AddChildBefore(ChildIterator it, std::shared_ptr<LuaAstNode> child);

	LuaAstNodeType _type;
	LuaTokenType _tokenType;
	std::string_view _text;
	TextRange _textRange;

	ChildrenContainer _children;
	std::weak_ptr<LuaAstNode> _parent;
};


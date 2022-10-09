#include "LuaParser/LuaAstNode/LuaAstNode.h"
#include "LuaParser/LuaAstVisitor.h"

LuaAstNode::LuaAstNode(LuaAstNodeType type, std::string_view text, TextRange range, LuaTokenType tokenType)
	: _type(type),
	  _tokenType(tokenType),
	  _text(text),
	  _textRange(range)

{
}

LuaAstNode::LuaAstNode(LuaAstNodeType type, LuaToken& token)
	: LuaAstNode(type, token.Text, token.Range, token.TokenType)
{
}

LuaAstNode::~LuaAstNode()
{
}

std::shared_ptr<LuaAstNode> LuaAstNode::Copy()
{
	auto copyNode = std::make_shared<LuaAstNode>(GetType(), _text, _textRange);
	copyNode->_children = _children;
	copyNode->_parent = _parent;
	return copyNode;
}

std::shared_ptr<LuaAstNode> LuaAstNode::FindFirstOf(LuaAstNodeType type)
{
	for (auto& child : _children)
	{
		if (child->GetType() == type)
		{
			return child;
		}
	}

	return nullptr;
}

void LuaAstNode::AcceptChildren(LuaAstVisitor& visitor)
{
	for (auto& child : _children)
	{
		visitor.Visit(child);
	}
}

void LuaAstNode::Accept(LuaAstVisitor& visitor)
{
	visitor.Visit(shared_from_this());
}

TextRange LuaAstNode::GetTextRange() const
{
	return _textRange;
}

std::string_view LuaAstNode::GetText() const
{
	return _text;
}

LuaAstNode::ChildrenContainer& LuaAstNode::GetChildren()
{
	return _children;
}

void LuaAstNode::AddChild(std::shared_ptr<LuaAstNode> child)
{
	if (child == nullptr)
	{
		return;
	}

	if (child->_textRange.IsEmpty())
	{
		return;
	}

	const auto source = GetSource();
	if (child->GetSource() != source)
	{
		return;
	}

	if (_textRange.IsEmpty())
	{
		_textRange = child->_textRange;
	}
	else
	{
		if (_textRange.StartOffset > child->_textRange.StartOffset)
		{
			_textRange.StartOffset = child->_textRange.StartOffset;
		}

		if (_textRange.EndOffset < child->_textRange.EndOffset)
		{
			_textRange.EndOffset = child->_textRange.EndOffset;
		}
	}

	_children.push_back(child);
	child->_parent = weak_from_this();
	_text = std::string_view(source + _textRange.StartOffset, _textRange.EndOffset - _textRange.StartOffset + 1);
}

LuaAstNodeType LuaAstNode::GetType() const
{
	return _type;
}

LuaTokenType LuaAstNode::GetTokenType() const
{
	return _tokenType;
}

void LuaAstNode::AddComment(std::shared_ptr<LuaAstNode> comment)
{
	auto childTextRange = comment->GetTextRange();

	// 防止无限递归
	if (_textRange.StartOffset == childTextRange.StartOffset && _textRange.EndOffset == childTextRange.EndOffset)
	{
		return;
	}

	if (_children.empty())
	{
		return AddChild(comment);
	}

	for (auto it = _children.rbegin(); it != _children.rend(); ++it)
	{
		auto currentChild = *it;
		auto currentChildTextRange = currentChild->GetTextRange();

		if (currentChildTextRange.Contain(childTextRange))
		{
			return currentChild->AddComment(comment);
		}
		else if (childTextRange.StartOffset > currentChildTextRange.EndOffset)
		{
			return AddChildBefore(it.base(), comment);
		}
		else if (currentChild == *_children.begin())
		{
			return AddChildBefore(_children.begin(), comment);
		}
	}
}

std::shared_ptr<LuaAstNode> LuaAstNode::GetParent()
{
	if (!_parent.expired())
	{
		return _parent.lock();
	}
	return nullptr;
}

const char* LuaAstNode::GetSource()
{
	if (_text.data() != nullptr)
	{
		return _text.data() - _textRange.StartOffset;
	}
	return nullptr;
}

void LuaAstNode::AddChildBefore(ChildIterator it, std::shared_ptr<LuaAstNode> child)
{
	if (it == _children.begin())
	{
		if (child->_textRange.IsEmpty())
		{
			return;
		}

		const auto source = GetSource();
		if (child->GetSource() != source)
		{
			return;
		}

		if (_textRange.IsEmpty())
		{
			_textRange = child->_textRange;
		}
		else
		{
			if (_textRange.StartOffset > child->_textRange.StartOffset)
			{
				_textRange.StartOffset = child->_textRange.StartOffset;
			}

			if (_textRange.EndOffset < child->_textRange.EndOffset)
			{
				_textRange.EndOffset = child->_textRange.EndOffset;
			}
		}
		_text = std::string_view(source + _textRange.StartOffset,
		                         _textRange.EndOffset - _textRange.StartOffset + 1);
	}
	else if (it == _children.end())
	{
		return AddChild(child);
	}

	_children.insert(it, child);
}

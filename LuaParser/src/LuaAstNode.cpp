#include "LuaParser/LuaAstNode.h"
#include "LuaParser/LuaAstVisitor.h"

LuaAstNode::LuaAstNode(LuaAstNodeType type, std::string_view text, TextRange range)
	: _type(type),
	  _text(text),
	  _textRange(range)
{
}

LuaAstNode::LuaAstNode(LuaAstNodeType type, LuaToken& token)
	: LuaAstNode(type, token.Text, token.Range)
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

const std::vector<std::shared_ptr<LuaAstNode>>& LuaAstNode::GetChildren()
{
	return _children;
}

void LuaAstNode::AddChild(std::shared_ptr<LuaAstNode> child)
{
	if (child == nullptr)
	{
		return;
	}
	auto source = GetSource();
	if (child->GetSource() != source)
	{
		return;
	}

	if (_textRange.StartOffset == 0 && _textRange.EndOffset == 0)
	{
		_textRange = child->_textRange;
	}

	else
	{
		if (child->_textRange.StartOffset == 0 && child->_textRange.EndOffset == 0)
		{
			return;
		}

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
	_text = std::string_view( source + _textRange.StartOffset, _textRange.EndOffset - _textRange.StartOffset + 1);
}

LuaAstNodeType LuaAstNode::GetType() const
{
	return _type;
}

void LuaAstNode::SetType(LuaAstNodeType type)
{
	_type = type;
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

	// 因为std::size_t 没法低于0
	for (int index = static_cast<int>(_children.size()) - 1; index >= 0; index--)
	{
		auto& currentChild = _children[index];
		auto currentChildTextRange = currentChild->GetTextRange();

		if (currentChildTextRange.Contain(childTextRange))
		{
			return currentChild->AddComment(comment);
		}
		else if (childTextRange.StartOffset > currentChildTextRange.EndOffset)
		{
			//此时一定在子节点空隙
			return AddChildAfter(index, comment);
		}
		else if (index == 0)
		{
			//此时在所有子节点之前
			return AddChildBefore(index, comment);
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

void LuaAstNode::AddChildAfter(int index, std::shared_ptr<LuaAstNode> child)
{
	if (index == static_cast<int>(_children.size() - 1))
	{
		AddChild(child);
	}
	else
	{
		auto it = _children.begin() + index + 1;
		_children.insert(it, child);
	}
}

void LuaAstNode::AddChildBefore(int index, std::shared_ptr<LuaAstNode> child)
{
	if (index == 0)
	{
		auto source = GetSource();
		if (child->GetSource() != source)
		{
			return;
		}

		if (_textRange.StartOffset == 0 && _textRange.EndOffset == 0)
		{
			_textRange = child->_textRange;
		}
		else
		{
			if (child->_textRange.StartOffset == 0 && child->_textRange.EndOffset == 0)
			{
				return;
			}

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

	auto it = _children.begin() + index;
	_children.insert(it, child);
}

#include "LuaParser/LuaAstNode.h"

LuaAstNode::LuaAstNode(LuaAstNodeType type, const char* source)
	: _type(type),
	  _text(source, 0),
	  _source(source),
	  _textRange(0, 0)
{
}

LuaAstNode::LuaAstNode(LuaAstNodeType type, LuaToken& token)
	: LuaAstNode(type, nullptr)
{
	auto text = token.Text;
	const char* source = text.data() - token.LuaTextRange.StartOffset;
	_source = source;
	_textRange = token.LuaTextRange;
	_text = token.Text;
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

	if (child->_source != _source)
	{
		return;
		// throw LuaParserException("child source not match parent source");
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
	_text = std::string_view(_source + _textRange.StartOffset, _textRange.EndOffset - _textRange.StartOffset + 1);
}

LuaAstNodeType LuaAstNode::GetType() const
{
	return _type;
}

void LuaAstNode::SetType(LuaAstNodeType type)
{
	_type = type;
}

void LuaAstNode::AddLeafChild(std::shared_ptr<LuaAstNode> child)
{
	auto childTextRange = child->GetTextRange();

	if (_children.empty())
	{
		return AddChild(child);
	}

	// 因为std::size_t 没法低于0
	for (int index = _children.size() - 1; index >= 0; index--)
	{
		auto& currentChild = _children[index];
		auto currentChildTextRange = currentChild->GetTextRange();

		if (currentChildTextRange.Contain(childTextRange))
		{
			return currentChild->AddLeafChild(child);
		}
		else if (childTextRange.StartOffset > currentChildTextRange.EndOffset)
		{
			//此时一定在子节点空隙
			return addChildAfter(index, child);
		}
		else if (index == 0)
		{
			//此时在所有子节点之前
			return addChildBefore(index, child);
		}
	}
}

void LuaAstNode::addChildAfter(int index, std::shared_ptr<LuaAstNode> child)
{
	if (index == (_children.size() - 1))
	{
		AddChild(child);
	}
	else
	{
		auto it = _children.begin() + index + 1;
		_children.insert(it, child);
	}
}

void LuaAstNode::addChildBefore(int index, std::shared_ptr<LuaAstNode> child)
{
	if (index == 0)
	{
		if (child->_source != _source)
		{
			return;
			// throw LuaParserException("child source not match parent source");
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
		_text = std::string_view(_source + _textRange.StartOffset, _textRange.EndOffset - _textRange.StartOffset + 1);
	}

	auto it = _children.begin() + index;
	_children.insert(it, child);
}

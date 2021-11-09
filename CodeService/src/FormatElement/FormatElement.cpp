#include "CodeService/FormatElement/FormatElement.h"

FormatElement::FormatElement(TextRange range)
	: _textRange(range)
{
}

FormatElement::~FormatElement()
{
}

FormatElementType FormatElement::GetType()
{
	return FormatElementType::FormatElement;
}

TextRange FormatElement::GetTextRange()
{
	return _textRange;
}


void FormatElement::AddChild(std::shared_ptr<FormatElement> child)
{
	if (static_cast<int>(child->GetType()) < static_cast<int>(FormatElementType::ControlStart))
	{
		const auto range = child->GetTextRange();
		AddTextRange(range);
	}
	_children.push_back(child);
}

void FormatElement::AddChildren(std::vector<std::shared_ptr<FormatElement>>& children)
{
	for(auto child:children)
	{
		AddChild(child);
	}
}

std::vector<std::shared_ptr<FormatElement>>& FormatElement::GetChildren()
{
	return _children;
}

bool FormatElement::HasValidTextRange() const
{
	return !(_textRange.StartOffset == 0 && _textRange.EndOffset == 0);
}

std::shared_ptr<FormatElement> FormatElement::LastValidElement() const
{
	for (auto it = _children.rbegin(); it != _children.rend(); it++)
	{
		if (static_cast<int>((*it)->GetType()) < static_cast<int>(FormatElementType::ControlStart))
		{
			return *it;
		}
	}

	return nullptr;
}

void FormatElement::Serialize(FormatContext& ctx, int position, FormatElement& parent)
{
	for (int i = 0; i < static_cast<int>(_children.size()); i++)
	{
		_children[i]->Serialize(ctx, i, *this);
	}
}

void FormatElement::Format(FormatContext& ctx)
{
	return Serialize(ctx, 0, *this);
}

void FormatElement::AddTextRange(TextRange range)
{
	if (_textRange.StartOffset == 0 && _textRange.EndOffset == 0)
	{
		_textRange = range;
	}

	if (range.StartOffset < _textRange.StartOffset)
	{
		_textRange.StartOffset = range.StartOffset;
	}

	if (range.EndOffset > _textRange.EndOffset)
	{
		_textRange.EndOffset = range.EndOffset;
	}
}

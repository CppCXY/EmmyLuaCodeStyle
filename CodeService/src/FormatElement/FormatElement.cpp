#include "CodeService/FormatElement/FormatElement.h"

FormatElement::FormatElement(TextRange range)
	: _textRange(range)
{
}

FormatElement::~FormatElement()
{
}

TextRange FormatElement::GetTextRange()
{
	return _textRange;
}


void FormatElement::AddChild(std::shared_ptr<FormatElement> child)
{
	if (static_cast<int>(child->GetType()) < static_cast<int>(FormatElementType::ControlStart)) {
		const auto range = child->GetTextRange();
		AddTextRange(range);
	}
	_children.push_back(child);
}

std::vector<std::shared_ptr<FormatElement>>& FormatElement::GetChildren()
{
	return _children;
}

bool FormatElement::HasValidTextRange() const
{
	return !(_textRange.StartOffset == 0 && _textRange.EndOffset == 0);
}

void FormatElement::Serialize(FormatContext& ctx, int position, FormatElement* parent)
{
	for (int i = 0; i != _children.size(); i++)
	{
		_children[i]->Serialize(ctx, i, this);
	}
}

void FormatElement::Format(FormatContext& ctx)
{
	return Serialize(ctx, 0, nullptr);
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

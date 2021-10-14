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

void FormatElement::AddChild(std::shared_ptr<LuaAstNode> node)
{
	const auto range = node->GetTextRange();
	AddTextRange(range);
}

void FormatElement::AddChild(std::shared_ptr<FormatElement> child)
{
	const auto range = child->GetTextRange();
	AddTextRange(range);
}

void FormatElement::Serialize(FormatContext& ctx)
{
}

void FormatElement::AddTextRange(TextRange range)
{
	if(_textRange.StartOffset == 0 && _textRange.EndOffset == 0)
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

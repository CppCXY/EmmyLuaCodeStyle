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

void FormatElement::AddChildren(ChildContainer& children)
{
	for (const auto& child : children)
	{
		AddChild(child);
	}
}

FormatElement::ChildContainer& FormatElement::GetChildren()
{
	return _children;
}

bool FormatElement::HasValidTextRange() const
{
	return !(_textRange.StartOffset == 0 && _textRange.EndOffset == 0);
}

std::shared_ptr<FormatElement> FormatElement::LastValidElement() const
{
	for (auto it = _children.rbegin(); it != _children.rend(); ++it)
	{
		if (static_cast<int>((*it)->GetType()) < static_cast<int>(FormatElementType::ControlStart))
		{
			return *it;
		}
	}

	return nullptr;
}

void FormatElement::Serialize(FormatContext& ctx, std::optional<FormatElement::ChildIterator> selfIt, FormatElement& parent)
{
	for (auto it = _children.begin(); it != _children.end(); ++it)
	{
		(*it)->Serialize(ctx, it, *this);
	}
}

void FormatElement::Diagnosis(DiagnosisContext& ctx, std::optional<FormatElement::ChildIterator> selfIt, FormatElement& parent)
{
	for (auto it = _children.begin(); it != _children.end(); ++it)
	{
		(*it)->Diagnosis(ctx, it, *this);
	}
} 

void FormatElement::Format(FormatContext& ctx)
{
	// workaround
	return Serialize(ctx, {}, *this);
}

void FormatElement::DiagnosisCodeStyle(DiagnosisContext& ctx)
{
	// workaround
	return Diagnosis(ctx, {}, *this);
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


int FormatElement::GetLastValidLine(FormatContext& ctx, ChildIterator it, FormatElement& parent)
{
	return ctx.GetLine(GetLastValidOffset(it, parent));
}

int FormatElement::GetNextValidLine(FormatContext& ctx, ChildIterator it, FormatElement& parent)
{
	const auto offset = GetNextValidOffset(it, parent);
	if (offset != -1)
	{
		return ctx.GetLine(offset);
	}
	else
	{
		return -1;
	}
}

int FormatElement::GetLastValidOffset(ChildIterator it, FormatElement& parent)
{
	auto& siblings = parent.GetChildren();
	auto rIt = std::reverse_iterator<decltype(it)>(it);

	for (; rIt != siblings.rend(); ++rIt)
	{

		auto sibling = *rIt;
		if (sibling->HasValidTextRange())
		{
			return sibling->GetTextRange().EndOffset;
		}
	}

	// 那么一定是往上找不到有效范围元素
	return parent.GetTextRange().StartOffset;
}

int FormatElement::GetNextValidOffset(ChildIterator it, FormatElement& parent)
{

	auto& siblings = parent.GetChildren();
	++it;
	for (; it != siblings.end(); ++it)
	{
		auto child = *it;
		if (child->HasValidTextRange())
		{
			return child->GetTextRange().StartOffset;
		}
	}

	return -1;
}

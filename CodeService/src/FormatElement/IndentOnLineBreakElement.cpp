#include "CodeService/FormatElement/IndentOnLineBreakElement.h"

#include "CodeService/FormatElement/KeepElement.h"

IndentOnLineBreakElement::IndentOnLineBreakElement()
	: FormatElement(),
	  _hasLineBreak(false)
{
}

FormatElementType IndentOnLineBreakElement::GetType()
{
	return FormatElementType::IndentOnLineBreakElement;
}

void IndentOnLineBreakElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	for (auto it = _children.begin(); it != _children.end(); ++it)
	{
		auto& child = *it;
		if (ctx.GetCharacterCount() == 0 && !_hasLineBreak)
		{
			_hasLineBreak = true;
			ctx.AddIndent();
		}

		if (child->Is(FormatElementType::KeepElement))
		{
			auto keepElement = std::dynamic_pointer_cast<KeepElement>(child);
			keepElement->AllowBreakLineSerialize(ctx, it, *this);
		}
		else
		{
			child->Serialize(ctx, it, *this);
		}
	}
	if (_hasLineBreak)
	{
		ctx.RecoverIndent();
	}
}

void IndentOnLineBreakElement::Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	for (auto it = _children.begin(); it != _children.end(); ++it)
	{
		if (ctx.GetCharacterCount() == 0 && !_hasLineBreak)
		{
			_hasLineBreak = true;
			ctx.AddIndent();
		}
		(*it)->Diagnosis(ctx, it, *this);
	}
	if (_hasLineBreak)
	{
		ctx.RecoverIndent();
	}
}

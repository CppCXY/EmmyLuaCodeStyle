#include "CodeService/FormatElement/AlignToFirstElement.h"

AlignToFirstElement::AlignToFirstElement(int lowestIndent)
	: _lowestIndent(lowestIndent)
{
}

FormatElementType AlignToFirstElement::GetType()
{
	return FormatElementType::AlignToFirstElement;
}

void AlignToFirstElement::Serialize(FormatContext& ctx, std::optional<FormatElement::ChildIterator> selfIt, FormatElement& parent)
{
	for (auto it = _children.begin(); it != _children.end(); ++it)
	{
		auto child = *it;
		if (it == _children.begin())
		{
			auto writeCount = ctx.GetCharacterCount();
			auto indentCount = ctx.GetCurrentIndent();
			if (writeCount > indentCount)
			{
				ctx.AddIndent(static_cast<int>(writeCount));
			}
			else
			{
				indentCount += _lowestIndent;
				const auto column = ctx.GetColumn(child->GetTextRange().StartOffset);
				if (column > static_cast<int>(indentCount))
				{
					ctx.AddIndent(column);
				}
				else
				{
					ctx.AddIndent(static_cast<int>(indentCount));
				}
			}
		}

		child->Serialize(ctx, it, *this);
	}

	if (!_children.empty()) {
		ctx.RecoverIndent();
	}
}

void AlignToFirstElement::Diagnosis(DiagnosisContext& ctx, std::optional<FormatElement::ChildIterator> selfIt, FormatElement& parent)
{
	for (auto it = _children.begin(); it != _children.end(); ++it)
	{
		auto child = *it;
		if (it == _children.begin())
		{
			auto writeCount = ctx.GetCharacterCount();
			auto indentCount = ctx.GetCurrentIndent();
			if (writeCount > indentCount)
			{
				ctx.AddIndent(static_cast<int>(writeCount));
			}
			else
			{
				indentCount += _lowestIndent;
				const auto column = ctx.GetColumn(child->GetTextRange().StartOffset);
				if (column > static_cast<int>(indentCount))
				{
					ctx.AddIndent(column);
				}
				else
				{
					ctx.AddIndent(static_cast<int>(indentCount));
				}
			}
		}

		child->Diagnosis(ctx, it, *this);
	}

	if (!_children.empty()) {
		ctx.RecoverIndent();
	}
}

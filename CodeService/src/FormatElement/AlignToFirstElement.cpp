#include "CodeService/FormatElement/AlignToFirstElement.h"

AlignToFirstElement::AlignToFirstElement(int lowestIndent)
	: _lowestIndent(lowestIndent)
{
}

FormatElementType AlignToFirstElement::GetType()
{
	return FormatElementType::AlignToFirstElement;
}

void AlignToFirstElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
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
				ctx.AddIndent(writeCount, ctx.GetOptions().indent_style);
			}
			else
			{
				indentCount += _lowestIndent;
				const auto column = ctx.GetColumn(child->GetTextRange().StartOffset);
				if (column > static_cast<int>(indentCount))
				{
					ctx.AddIndent(column, ctx.GetOptions().indent_style);
				}
				else
				{
					ctx.AddIndent(indentCount, ctx.GetOptions().indent_style);
				}
			}
		}

		child->Serialize(ctx, it, *this);
	}

	if (!_children.empty())
	{
		ctx.RecoverIndent();
	}
}

void AlignToFirstElement::Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
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
				ctx.AddIndent(writeCount, ctx.GetOptions().indent_style);
			}
			else
			{
				indentCount += _lowestIndent;
				const auto column = ctx.GetColumn(child->GetTextRange().StartOffset);
				if (column > static_cast<int>(indentCount))
				{
					ctx.AddIndent(column, ctx.GetOptions().indent_style);
				}
				else
				{
					ctx.AddIndent(indentCount, ctx.GetOptions().indent_style);
				}
			}
		}

		child->Diagnosis(ctx, it, *this);
	}

	if (!_children.empty())
	{
		ctx.RecoverIndent();
	}
}

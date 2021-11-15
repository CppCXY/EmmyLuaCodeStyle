#include "CodeService/FormatElement/AlignToFirstElement.h"

AlignToFirstElement::AlignToFirstElement(int lowestIndent)
	: _lowestIndent(lowestIndent)
{
}

FormatElementType AlignToFirstElement::GetType()
{
	return FormatElementType::AlignToFirstElement;
}

void AlignToFirstElement::Serialize(FormatContext& ctx, int position, FormatElement& parent)
{
	for (int i = 0; i != static_cast<int>(_children.size()); i++)
	{
		if (i == 0)
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
				auto column = ctx.GetColumn(_children[i]->GetTextRange().StartOffset);
				if (column > indentCount)
				{
					ctx.AddIndent(column);
				}
				else
				{
					ctx.AddIndent(static_cast<int>(indentCount));
				}
			}
		}

		_children[i]->Serialize(ctx, i, *this);

		if(i == _children.size() - 1)
		{
			ctx.RecoverIndent();
		}
	}
}

void AlignToFirstElement::Diagnosis(DiagnosisContext& ctx, int position, FormatElement& parent)
{
	for (int i = 0; i != static_cast<int>(_children.size()); i++)
	{
		if (i == 0)
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
				auto column = ctx.GetColumn(_children[i]->GetTextRange().StartOffset);
				if (column > indentCount)
				{
					ctx.AddIndent(column);
				}
				else
				{
					ctx.AddIndent(static_cast<int>(indentCount));
				}
			}
		}

		_children[i]->Diagnosis(ctx, i, *this);

		if (i == _children.size() - 1)
		{
			ctx.RecoverIndent();
		}
	}
}

#include "CodeService/FormatElement/IndentElement.h"
#include "Util/format.h"

IndentElement::IndentElement(int specialIndent)
	: FormatElement(TextRange(0, 0)),
	  _specialIndent(specialIndent)
{
}

FormatElementType IndentElement::GetType()
{
	return FormatElementType::IndentElement;
}

void IndentElement::Serialize(FormatContext& ctx, int position, FormatElement& parent)
{
	ctx.AddIndent(_specialIndent);

	FormatElement::Serialize(ctx, position, parent);

	ctx.RecoverIndent();
}

void IndentElement::Diagnosis(DiagnosisContext& ctx, int position, FormatElement& parent)
{
	ctx.AddIndent(_specialIndent);

	for (int i = 0; i < static_cast<int>(_children.size()); i++)
	{
		auto child = _children[i];

		if (child->HasValidTextRange() && !ctx.GetOptions().use_tab)
		{
			auto range = child->GetTextRange();
			auto character = ctx.GetColumn(range.StartOffset);
			if (character != static_cast<int>(ctx.GetCurrentIndent()))
			{
				ctx.PushDiagnosis(format("Incorrect indentation {}", character),
				                  TextRange(range.StartOffset, range.StartOffset));
			}
		}

		_children[i]->Diagnosis(ctx, i, *this);
	}

	ctx.RecoverIndent();
}

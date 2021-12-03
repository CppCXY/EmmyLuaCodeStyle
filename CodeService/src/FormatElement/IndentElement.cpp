#include "CodeService/FormatElement/IndentElement.h"
#include "Util/format.h"
#include "CodeService/FormatElement/FormatElement.h"

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

		if (child->HasValidTextRange() && ctx.GetOptions().indent_style == IndentStyle::Space && child->GetType() != FormatElementType::IndentElement)
		{
			auto range = child->GetTextRange();
			auto character = ctx.GetColumn(range.StartOffset);
			if (character != static_cast<int>(ctx.GetCurrentIndent()))
			{
				auto line = ctx.GetLine(range.StartOffset);
				ctx.PushDiagnosis(format(LText("incorrect indentation {}, here need {} indent"),
				                         character, ctx.GetCurrentIndent()),
				                  LuaDiagnosisPosition(line, 0),
				                  LuaDiagnosisPosition(line, character)
				);
			}
		}

		_children[i]->Diagnosis(ctx, i, *this);
	}

	ctx.RecoverIndent();
}

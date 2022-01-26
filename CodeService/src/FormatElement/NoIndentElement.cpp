#include "CodeService/FormatElement/NoIndentElement.h"
#include "Util/format.h"

NoIndentElement::NoIndentElement()
	: FormatElement()
{
}

FormatElementType NoIndentElement::GetType()
{
	return FormatElementType::NoIndentElement;
}

void NoIndentElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	ctx.AddIndent(ctx.GetLastIndent(), ctx.GetOptions().indent_style);

	FormatElement::Serialize(ctx, selfIt, parent);

	ctx.RecoverIndent();
}

void NoIndentElement::Diagnosis(DiagnosisContext& ctx, ChildIterator shared_ptr, FormatElement& parent)
{
	ctx.AddIndent(ctx.GetLastIndent(), ctx.GetOptions().indent_style);

	for (auto it = _children.begin(); it != _children.end(); ++it)
	{
		const auto child = *it;

		if (child->HasValidTextRange() && ctx.GetOptions().indent_style == IndentStyle::Space && child->GetType() !=
			FormatElementType::IndentElement)
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

		child->Diagnosis(ctx, it, *this);
	}

	ctx.RecoverIndent();
}

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

void IndentElement::Serialize(FormatContext& ctx, std::optional<FormatElement::ChildIterator> selfIt, FormatElement& parent)
{
	ctx.AddIndent(_specialIndent);

	FormatElement::Serialize(ctx, selfIt, parent);

	ctx.RecoverIndent();
}

void IndentElement::Diagnosis(DiagnosisContext& ctx, std::optional<FormatElement::ChildIterator> selfIt, FormatElement& parent)
{
	ctx.AddIndent(_specialIndent);

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

#include "CodeService/FormatElement/IndentElement.h"
#include "Util/format.h"
#include "CodeService/FormatElement/FormatElement.h"

IndentElement::IndentElement()
	: FormatElement(),
	  _specialIndent(0),
	  _style(IndentStyle::Space),
	  _defaultIndent(true)
{
}

IndentElement::IndentElement(std::size_t specialIndent, IndentStyle style)
	: FormatElement(),
	  _specialIndent(specialIndent),
	  _style(style),
	  _defaultIndent(false)
{
}

FormatElementType IndentElement::GetType()
{
	return FormatElementType::IndentElement;
}

void IndentElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	if (_defaultIndent)
	{
		ctx.AddIndent();
	}
	else
	{
		ctx.AddIndent(_specialIndent, _style);
	}

	FormatElement::Serialize(ctx, selfIt, parent);

	ctx.RecoverIndent();
}

void IndentElement::Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	if (_defaultIndent)
	{
		ctx.AddIndent();
	}
	else
	{
		ctx.AddIndent(_specialIndent, _style);
	}

	for (auto it = _children.begin(); it != _children.end(); ++it)
	{
		const auto child = *it;

		if (child->HasValidTextRange() && ctx.GetOptions().indent_style == IndentStyle::Space
			&& child->GetType() != FormatElementType::IndentElement && child->GetType() !=
			FormatElementType::NoIndentElement)
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

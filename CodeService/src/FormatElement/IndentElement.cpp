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
	else if (_style == IndentStyle::Tab)
	{
		ctx.AddIndent(FormatContext::IndentState{0, _specialIndent, IndentStyle::Tab});
	}
	else
	{
		ctx.AddIndent(FormatContext::IndentState{_specialIndent, 0, IndentStyle::Space});
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
	else if (_style == IndentStyle::Tab)
	{
		ctx.AddIndent(FormatContext::IndentState{0, _specialIndent, IndentStyle::Tab});
	}
	else
	{
		ctx.AddIndent(FormatContext::IndentState{_specialIndent, 0, IndentStyle::Space});
	}


	for (auto it = _children.begin(); it != _children.end(); ++it)
	{
		const auto child = *it;

		if (child->HasValidTextRange()
			&& child->GetType() != FormatElementType::IndentElement
			&& child->GetType() != FormatElementType::NoIndentElement)
		{
			auto range = child->GetTextRange();
			auto line = ctx.GetLine(range.StartOffset);
			auto character = ctx.GetColumn(range.StartOffset);
			auto indentState = ctx.CalculateIndentState(range.StartOffset);
			auto state = ctx.GetCurrentIndent();
			if (ctx.GetOptions().indent_style != indentState.IndentStyle)
			{
				ctx.PushDiagnosis(
					format(LText("incorrect indentation style, expect {}, but here is {}"),
					       GetIndentStyleName(state.IndentStyle),
					       GetIndentStyleName(indentState.IndentStyle)
					),
					LuaDiagnosisPosition(line, 0),
					LuaDiagnosisPosition(line, character)
				);
				goto endIndentDiagnose;
			}

			if (indentState.IndentStyle == IndentStyle::Space) {
				if (state.SpaceIndent != indentState.SpaceIndent)
				{
					ctx.PushDiagnosis(
						format(LText("incorrect indentation {}, here need {} space indentation"),
							indentState.SpaceIndent, state.SpaceIndent),
						LuaDiagnosisPosition(line, 0),
						LuaDiagnosisPosition(line, character)
					);
				}
				goto endIndentDiagnose;
			}
			else
			{
				if (state.SpaceIndent != indentState.SpaceIndent || state.TabIndent != indentState.TabIndent)
				{
					ctx.PushDiagnosis(
						format(LText("incorrect indentation, here need {} tab and {} space indentation"),
							state.TabIndent, state.SpaceIndent),
						LuaDiagnosisPosition(line, 0),
						LuaDiagnosisPosition(line, character)
					);
				}
			}
		}

	endIndentDiagnose:
		child->Diagnosis(ctx, it, *this);
	}

	ctx.RecoverIndent();
}

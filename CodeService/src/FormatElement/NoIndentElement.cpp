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
	ctx.AddIndent(ctx.GetLastIndent());

	FormatElement::Serialize(ctx, selfIt, parent);

	ctx.RecoverIndent();
}

void NoIndentElement::Diagnosis(DiagnosisContext& ctx, ChildIterator shared_ptr, FormatElement& parent)
{
	ctx.AddIndent(ctx.GetLastIndent());


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

			if (indentState.IndentStyle == IndentStyle::Space)
			{
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

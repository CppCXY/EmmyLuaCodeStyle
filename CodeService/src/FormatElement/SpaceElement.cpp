#include "CodeService/FormatElement/SpaceElement.h"

#include "CodeService/FormatElement/KeyWordElement.h"
#include "CodeService/FormatElement/OperatorElement.h"
#include "Util/format.h"

SpaceElement::SpaceElement(int space)
	: FormatElement(),
	  _space(space)
{
}

FormatElementType SpaceElement::GetType()
{
	return FormatElementType::SpaceElement;
}

void SpaceElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	int nextOffset = GetNextValidOffset(selfIt, parent);
	if (nextOffset != -1)
	{
		ctx.PrintBlank(_space);
	}
}

void SpaceElement::Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	const int lastOffset = GetLastValidOffset(selfIt, parent);
	const int nextOffset = GetNextValidOffset(selfIt, parent);

	if (nextOffset == -1 && lastOffset != -1)
	{
		return;
	}

	const int lastElementLine = ctx.GetLine(lastOffset);
	const int nextElementLine = ctx.GetLine(nextOffset);

	if (nextElementLine == lastElementLine)
	{
		PushSpaceDiagnostic(ctx, GetLastValidElement(selfIt, parent), GetNextValidElement(selfIt, parent));
	}
	else
	{
		auto character = ctx.GetColumn(lastOffset);
		ctx.PushDiagnosis(LText("should be whitespace"),
		                  LuaDiagnosisPosition(lastElementLine, character + 1),
		                  LuaDiagnosisPosition(lastElementLine, character + 1),
		                  DiagnosisType::Blank);
	}
}

void SpaceElement::PushSpaceDiagnostic(DiagnosisContext& ctx,
                                       std::shared_ptr<FormatElement> left,
                                       std::shared_ptr<FormatElement> right)
{
	if (left == nullptr || right == nullptr)
	{
		return;
	}

	auto leftOffset = left->GetTextRange().EndOffset;
	auto rightOffset = right->GetTextRange().StartOffset;
	int diff = rightOffset - leftOffset - 1;

	if (diff < 0 || _space < 0 || diff == _space)
	{
		return;
	}

	auto additional = GetAdditionalNote(left, right);
	switch (_space)
	{
	case 0:
		{
			ctx.PushDiagnosis(Util::format(LText("unnecessary whitespace {}"), additional),
			                  TextRange(leftOffset, rightOffset),
			                  DiagnosisType::Blank);
			break;
		}
	case 1:
		{
			if (diff == 0)
			{
				ctx.PushDiagnosis(Util::format(LText("missing whitespace {}"), additional),
				                  TextRange(leftOffset, rightOffset),
				                  DiagnosisType::Blank);
			}
			else // diff > 1
			{
				ctx.PushDiagnosis(Util::format(LText("multiple spaces {}"), additional),
				                  TextRange(leftOffset + 1, rightOffset - 1),
				                  DiagnosisType::Blank);
			}
			break;
		}
	default:
		{
			if (diff < _space)
			{
				ctx.PushDiagnosis(Util::format(LText("expected {} whitespace, found {} {}"), _space, diff, additional),
				                  TextRange(leftOffset, rightOffset),
				                  DiagnosisType::Blank);
			}
			else // diff > _space
			{
				ctx.PushDiagnosis(Util::format(LText("expected {} whitespace, found {} {}"), _space, diff, additional),
				                  TextRange(leftOffset + 1, rightOffset - 1),
				                  DiagnosisType::Blank);
			}
			break;
		}
	}
}

std::string SpaceElement::GetAdditionalNote(std::shared_ptr<FormatElement> left, std::shared_ptr<FormatElement> right)
{
	if (left->Is(FormatElementType::KeyWordElement))
	{
		auto keyWord = std::dynamic_pointer_cast<KeyWordElement>(left);
		return Util::format(LText("after keyword '{}'"), keyWord->GetText());
	}
	else if (left->Is(FormatElementType::OperatorElement))
	{
		auto operatorElement = std::dynamic_pointer_cast<OperatorElement>(left);
		return Util::format(LText("after operator '{}'"), operatorElement->GetText());
	}
	else if (right->Is(FormatElementType::KeyWordElement))
	{
		auto keyWord = std::dynamic_pointer_cast<KeyWordElement>(right);
		return Util::format(LText("before keyword '{}'"), keyWord->GetText());
	}
	else if (right->Is(FormatElementType::OperatorElement))
	{
		auto operatorElement = std::dynamic_pointer_cast<OperatorElement>(right);
		return Util::format(LText("before operator '{}'"), operatorElement->GetText());
	}
	else
	{
		return "";
	}
}

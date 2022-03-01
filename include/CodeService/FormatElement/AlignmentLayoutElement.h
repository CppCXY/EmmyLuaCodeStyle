#pragma once

#include "FormatElement.h"
#include "FormatElementEnum.h"

class AlignmentLayoutElement : public FormatElement
{
public:
	AlignmentLayoutElement(std::string_view alignSign, AlignStyleLevel level = AlignStyleLevel::None);
	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnose(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
private:
	int GetAlignPosition(FormatContext& ctx);
	// int GetAlignOffset(FormatContext& ctx);
	int GetAlignOffsetWithWeakRule(FormatContext& ctx);
	void AlignmentSerialize(SerializeContext& ctx, ChildIterator selfIt, int alignSignPosition,
				FormatElement& parent);
	void AlignmentDiagnosis(DiagnosisContext& ctx, ChildIterator selfIt, int alignSignPosition,
				FormatElement& parent);
	std::string _alignSign;
	AlignStyleLevel _level;
};

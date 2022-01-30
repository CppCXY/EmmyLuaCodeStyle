#pragma once

#include "FormatElement.h"

class AlignmentLayoutElement : public FormatElement
{
public:
	AlignmentLayoutElement(std::string_view alignSign);
	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
private:
	int GetAlignPosition(FormatContext& ctx);
	void AlignmentSerialize(SerializeContext& ctx, ChildIterator selfIt, int alignSignPosition,
				FormatElement& parent);
	void AlignmentDiagnosis(DiagnosisContext& ctx, ChildIterator selfIt, int alignSignPosition,
				FormatElement& parent);
	std::string _alignSign;
};

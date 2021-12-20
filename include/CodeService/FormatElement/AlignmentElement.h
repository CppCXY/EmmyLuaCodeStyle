#pragma once

#include "FormatElement.h"

class AlignmentElement : public FormatElement
{
public:
	AlignmentElement(int alignmentPosition);
	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, std::optional<ChildIterator> selfIt, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, std::optional<ChildIterator> selfIt, FormatElement& parent) override;
private:
	int _alignmentPosition;
};

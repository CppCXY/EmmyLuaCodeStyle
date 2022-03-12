#pragma once

#include "FormatElement.h"

class AlignmentElement : public FormatElement
{
public:
	AlignmentElement(int alignmentPosition);
	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnose(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
private:
	int _alignmentPosition;
};

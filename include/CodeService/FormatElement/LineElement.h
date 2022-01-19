#pragma once

#include "FormatElement.h"

class LineElement : public FormatElement
{
public:
	LineElement();

	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
};

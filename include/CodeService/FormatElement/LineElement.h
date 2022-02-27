#pragma once

#include "FormatElement.h"

class LineElement : public FormatElement
{
public:
	LineElement();

	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnose(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
};

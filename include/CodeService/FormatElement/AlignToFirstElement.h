#pragma once

#include "FormatElement.h"

class AlignToFirstElement : public FormatElement
{
public:
	AlignToFirstElement();

	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnose(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
};

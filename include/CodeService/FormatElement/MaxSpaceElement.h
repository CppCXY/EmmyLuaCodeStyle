#pragma once

#include "FormatElement.h"

class MaxSpaceElement : public FormatElement
{
public:
	MaxSpaceElement(int space);

	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnose(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
private:
	int _space;
};

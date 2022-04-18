#pragma once

#include "FormatElement.h"

class MaxLineElement: public FormatElement
{
public:
	MaxLineElement(int line);

	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
private:
	int _line;
};

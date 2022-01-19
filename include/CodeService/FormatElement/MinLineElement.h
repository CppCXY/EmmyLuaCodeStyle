#pragma once

#include "FormatElement.h"

class MinLineElement : public FormatElement
{
public:
	MinLineElement(int line);

	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
private:
	int _line;
};

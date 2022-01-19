#pragma once

#include "FormatElement.h"

class IndentElement : public FormatElement
{
public:
	IndentElement(int specialIndent = -1);

	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, ChildIterator& selfIt, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, ChildIterator& selfIt, FormatElement& parent) override;
private:
	int _specialIndent;
};

#pragma once

#include "FormatElement.h"

class KeepLineElement : public FormatElement
{
public:
	explicit KeepLineElement(int line = -1);

	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;

	void Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;

private:
	int _line;
};



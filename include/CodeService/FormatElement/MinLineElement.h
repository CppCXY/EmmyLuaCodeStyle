#pragma once

#include "FormatElement.h"

class MinLineElement : public FormatElement
{
public:
	MinLineElement(int line);

	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnose(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
private:
	int _line;
};

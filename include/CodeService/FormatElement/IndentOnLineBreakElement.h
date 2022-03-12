#pragma once

#pragma once

#include "FormatElement.h"

class IndentOnLineBreakElement : public FormatElement
{
public:
	IndentOnLineBreakElement();

	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnose(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
private:
	bool _hasLineBreak;
};

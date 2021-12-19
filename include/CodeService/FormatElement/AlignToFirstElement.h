#pragma once

#include "FormatElement.h"

class AlignToFirstElement : public FormatElement
{
public:
	AlignToFirstElement(int lowestIndent);

	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, std::optional<ChildIterator> selfIt, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, std::optional<ChildIterator> selfIt, FormatElement& parent) override;
private:
	int _lowestIndent;
};

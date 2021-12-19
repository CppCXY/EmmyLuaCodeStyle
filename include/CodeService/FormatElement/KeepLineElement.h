#pragma once

#include "FormatElement.h"

class KeepLineElement : public FormatElement
{
public:
	explicit KeepLineElement(int line = -1);

	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, std::optional<ChildIterator> selfIt, FormatElement& parent) override;

	void Diagnosis(DiagnosisContext& ctx, std::optional<ChildIterator> selfIt, FormatElement& parent) override;

private:
	int _line;
};



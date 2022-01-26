#pragma once
#include "FormatElement.h"

class KeepBlankElement : public FormatElement
{
public:
	KeepBlankElement(int blank);

	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
private:
	int _blank;
};

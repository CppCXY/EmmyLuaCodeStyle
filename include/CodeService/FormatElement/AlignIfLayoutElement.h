#pragma once

#include "FormatElement.h"

class AlignIfElement : public FormatElement
{
public:
	AlignIfElement();
	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
private:
	void AlignElement(FormatContext& ctx);
	void AlignConditionExpression(FormatContext& ctx, FormatElement& parent);
};

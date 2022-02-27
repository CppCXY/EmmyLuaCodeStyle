#pragma once

#include "FormatElement.h"

class AlignIfElement : public FormatElement
{
public:
	AlignIfElement();
	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnose(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
private:
	bool ElseifFounded();
	bool OnlyEmptyCharBeforeAnd(FormatContext& ctx, FormatElement& expressionLayout);
	void AlignElement(FormatContext& ctx);
	void AlignConditionExpression(FormatContext& ctx, FormatElement& parent, int spacePositionAfterIndent);

};

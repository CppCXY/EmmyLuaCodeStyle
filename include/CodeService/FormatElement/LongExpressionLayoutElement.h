#pragma once

#include "FormatElement.h"

class LongExpressionLayoutElement : public FormatElement
{
public:
	LongExpressionLayoutElement(int continuationIndent, bool isAssignLeftExprList = false);

	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, ChildIterator& selfIt, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, ChildIterator& selfIt, FormatElement& parent) override;
private:
	void SerializeSubExpression(FormatContext& ctx, FormatElement& parent);
	void DiagnosisSubExpression(DiagnosisContext& ctx, FormatElement& parent);

	bool _hasContinuation;
	int _continuationIndent;
};

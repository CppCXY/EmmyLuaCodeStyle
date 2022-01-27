#pragma once

#include "FormatElement.h"

class LongExpressionLayoutElement : public FormatElement
{
public:
	LongExpressionLayoutElement(int continuationIndent);

	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
private:
	void SerializeSubExpression(SerializeContext& ctx, FormatElement& parent);
	void DiagnosisSubExpression(DiagnosisContext& ctx, FormatElement& parent);

	bool _hasContinuation;
	int _continuationIndent;
};

#pragma once

#include "FormatElement.h"

class LongExpressionLayoutElement : public FormatElement
{
public:
	LongExpressionLayoutElement(int continuationIndent = -1);

	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnose(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
private:
	void SerializeSubExpression(SerializeContext& ctx, FormatElement& parent);
	void DiagnosisSubExpression(DiagnosisContext& ctx, FormatElement& parent);
	void IndentSubExpression(FormatContext& ctx);

	bool _hasContinuation;
	int _continuationIndent;
};

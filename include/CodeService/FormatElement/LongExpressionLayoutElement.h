#pragma once

#include "FormatElement.h"

class LongExpressionLayoutElement : public FormatElement
{
public:
	LongExpressionLayoutElement(int continuationIndent);

	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, int position, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, int position, FormatElement& parent) override;
private:
	void SerializeSubExpression(FormatContext& ctx, FormatElement& parent);
	void DiagnosisSubExpression(DiagnosisContext& ctx, FormatElement& parent);

	bool _hasContinuation;
	int _continuationIndent;
};

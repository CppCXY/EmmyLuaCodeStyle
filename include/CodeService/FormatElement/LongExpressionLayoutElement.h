#pragma once

#include "FormatElement.h"

class LongExpressionLayoutElement : public FormatElement
{
public:
	LongExpressionLayoutElement();

	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, int position, FormatElement* parent) override;

private:
	void SerializeSubExpression(FormatContext& ctx, FormatElement* parent);

	bool _hasIndent;
};

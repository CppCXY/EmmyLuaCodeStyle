#pragma once

#include "FormatElement.h"

class MinLineElement : public FormatElement
{
public:
	MinLineElement(int line);

	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, int position, FormatElement& parent) override;
	void Diagnosis(FormatContext& ctx, int position, FormatElement& parent) override;
private:
	int _line;
};

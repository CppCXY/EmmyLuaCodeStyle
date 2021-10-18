#pragma once

#
#include "FormatElement.h"

class KeepBlankElement : public FormatElement
{
public:
	KeepBlankElement(int blank);

	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, int position, FormatElement* parent) override;
private:
	int _blank;
};

#pragma once

#include "FormatElement.h"

class BreakableElement : FormatElement
{
public:
	BreakableElement();

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
protected:

};
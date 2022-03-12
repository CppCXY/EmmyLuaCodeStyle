#pragma once

#include "FormatElement.h"

class NoIndentElement: public FormatElement
{
public:
	NoIndentElement();

	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnose(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;

};
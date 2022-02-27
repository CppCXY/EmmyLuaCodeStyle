#pragma once

#include <vector>
#include "FormatElement.h"

class StatementElement : public FormatElement
{
public:
	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnose(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
};

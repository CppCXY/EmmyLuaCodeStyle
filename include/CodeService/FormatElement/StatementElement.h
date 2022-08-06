#pragma once

#include <vector>
#include "FormatElement.h"

class StatementElement : public FormatElement
{
public:
	StatementElement();

	FormatElementType GetType() override;

	void SetDisableFormat();

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
private:
	bool _disableFormat;
};

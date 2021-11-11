#pragma once

#include "FormatElement.h"

class KeepLineElement : public FormatElement
{
public:
	explicit KeepLineElement(int line = -1);

	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, int position, FormatElement& parent) override;

	void Diagnosis(DiagnosisContext& ctx, int position, FormatElement& parent) override;

private:
	int _line;
};



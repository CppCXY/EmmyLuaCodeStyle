#pragma once

#include "FormatElement.h"

class AlignmentLayoutElement : public FormatElement
{
public:
	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, int position, FormatElement& parent) override;
	void Diagnosis(FormatContext& ctx, int position, FormatElement& parent) override;
private:
	int getAlignPosition(FormatContext& ctx, int position, FormatElement& parent);
	void alignmentSerialize(FormatContext& ctx, int position, FormatElement& parent, int eqPosition);
	void alignmentDiagnosis(FormatContext& ctx, int position, FormatElement& parent, int eqPosition);
};
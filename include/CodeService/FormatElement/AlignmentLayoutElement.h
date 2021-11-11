#pragma once

#include "FormatElement.h"

class AlignmentLayoutElement : public FormatElement
{
public:
	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, int position, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, int position, FormatElement& parent) override;
private:
	int getAlignPosition(std::shared_ptr<LuaParser> luaParser);
	void alignmentSerialize(FormatContext& ctx, int position, FormatElement& parent, int eqPosition);
	void alignmentDiagnosis(DiagnosisContext& ctx, int position, FormatElement& parent, int eqPosition);
};

#pragma once

#include "FormatElement.h"

class AlignmentLayoutElement : public FormatElement
{
public:
	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
private:
	int GetAlignPosition(std::shared_ptr<LuaParser> luaParser);
	void AlignmentSerialize(SerializeContext& ctx, ChildIterator selfIt, int eqPosition,
	                        FormatElement& parent);
	void AlignmentDiagnosis(DiagnosisContext& ctx, ChildIterator selfIt, int eqPosition,
	                        FormatElement& parent);
};

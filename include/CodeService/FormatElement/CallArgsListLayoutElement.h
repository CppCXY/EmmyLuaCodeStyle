#pragma once

#include "FormatElement.h"

class CallArgsListLayoutElement : public FormatElement
{
public:
	CallArgsListLayoutElement();

	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
private:

	void SerializeSubExpression(SerializeContext& ctx, std::shared_ptr<FormatElement> parent, bool topLevelSubexpression);
	void DiagnoseSubExpression(DiagnosisContext& ctx, std::shared_ptr<FormatElement> parent, bool topLevelSubexpression);

	bool IsLastArg(std::shared_ptr<FormatElement> node);
	bool IsFirstArg(std::shared_ptr<FormatElement> node);
	bool _hasLineBreak;

};

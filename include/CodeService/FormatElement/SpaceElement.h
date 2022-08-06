#pragma once
#include "FormatElement.h"

class SpaceElement : public FormatElement
{
public:
	SpaceElement(int space);

	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
protected:
	void PushSpaceDiagnostic(DiagnosisContext& ctx, std::shared_ptr<FormatElement> left, std::shared_ptr<FormatElement> right);
	std::string GetAdditionalNote(std::shared_ptr<FormatElement> left, std::shared_ptr<FormatElement> right);

	int _space;
};


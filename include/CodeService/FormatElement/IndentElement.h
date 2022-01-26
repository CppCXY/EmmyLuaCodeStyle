#pragma once

#include "FormatElement.h"

class IndentElement : public FormatElement
{
public:
	IndentElement();
	IndentElement(std::size_t specialIndent, IndentStyle style);

	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
private:
	std::size_t _specialIndent;
	IndentStyle _style;
	bool _defaultIndent;
};

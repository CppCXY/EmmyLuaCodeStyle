#pragma once

#include "FormatElement.h"
#include "TextDefineType.h"

class TextElement : public FormatElement
{
public:
	explicit TextElement(std::string_view text, TextDefineType textDefineType, TextRange range = TextRange());

	explicit TextElement(std::shared_ptr<LuaAstNode> node, TextDefineType textDefineType = TextDefineType::Normal);

	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;

	std::string_view GetText() const;
	void SetTextDefineType(TextDefineType textDefineType);
	
private:

	std::string_view _text;
	TextDefineType _textDefineType;
};

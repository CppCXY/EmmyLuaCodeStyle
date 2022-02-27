#pragma once

#include "FormatElement.h"
#include "FormatElementEnum.h"

class TextElement : public FormatElement
{
public:
	explicit TextElement(std::string_view text, TextRange range = TextRange(), TextSpaceType space = TextSpaceType::None);

	explicit TextElement(std::shared_ptr<LuaAstNode> node, TextSpaceType space = TextSpaceType::None);

	FormatElementType GetType() override;

	void SetSpaceType(TextSpaceType type);

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnose(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;

	std::string_view GetText() const;
private:
	std::string_view _text;
	TextSpaceType _space;
};

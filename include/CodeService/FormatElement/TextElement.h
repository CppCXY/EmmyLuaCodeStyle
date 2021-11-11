#pragma once

#include "FormatElement.h"
#include "LuaParser/LuaAstNode.h"

class TextElement : public FormatElement
{
public:
	explicit TextElement(std::string_view text, TextRange range = TextRange());

	explicit TextElement(std::shared_ptr<LuaAstNode> node);

	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, int position, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, int position, FormatElement& parent) override;

	std::string_view GetText() const;
private:
	std::string_view _text;
};
 
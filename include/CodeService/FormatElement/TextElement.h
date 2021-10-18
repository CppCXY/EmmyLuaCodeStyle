#pragma once

#include "FormatElement.h"
#include "LuaParser/LuaAstNode.h"

class TextElement : public FormatElement
{
public:
	explicit TextElement(std::string_view text, TextRange range = TextRange());

	explicit TextElement(std::shared_ptr<LuaAstNode> node);

	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, int position,FormatElement* parent) override;

private:
	std::string_view _text;
};
 
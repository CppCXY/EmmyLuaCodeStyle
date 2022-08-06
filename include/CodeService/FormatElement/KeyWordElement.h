#pragma once

#include "TextElement.h"

class KeyWordElement : public TextElement
{
public:
	KeyWordElement(std::shared_ptr<LuaAstNode> node);

	FormatElementType GetType() override;
};

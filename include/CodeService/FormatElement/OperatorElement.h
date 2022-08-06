#pragma once

#include "TextElement.h"

class OperatorElement : public TextElement
{
public:
	OperatorElement(std::shared_ptr<LuaAstNode> node);

	FormatElementType GetType() override;
};

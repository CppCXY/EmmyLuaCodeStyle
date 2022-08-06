#include "CodeService/FormatElement/KeyWordElement.h"

KeyWordElement::KeyWordElement(std::shared_ptr<LuaAstNode> node)
	: TextElement(node)
{
}

FormatElementType KeyWordElement::GetType()
{
	return FormatElementType::KeyWordElement;
}

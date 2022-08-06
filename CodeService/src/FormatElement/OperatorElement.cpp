#include "CodeService/FormatElement/OperatorElement.h"

OperatorElement::OperatorElement(std::shared_ptr<LuaAstNode> node)
	:TextElement(node)
{
}

FormatElementType OperatorElement::GetType()
{
	return FormatElementType::OperatorElement;
}

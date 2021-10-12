#include "CodeService/FormatElement/IndentElement.h"
#include "CodeService/FormatElement/KeepLineElement.h"
#include "CodeService/FormatElement/MinLineElement.h"

IndentElement::IndentElement()
{
}

FormatElementType IndentElement::GetType()
{
	return FormatElementType::IndentElement;
}

void IndentElement::AddChild(std::shared_ptr<FormatElement> child)
{
	
}

void IndentElement::AddChild(std::shared_ptr<LuaAstNode> node)
{
	
}

void IndentElement::KeepLine(int line)
{
	AddChild(std::make_shared<KeepLineElement>(line));
}

void IndentElement::MinLine(int line)
{
	AddChild(std::make_shared<MinLineElement>(line));
}


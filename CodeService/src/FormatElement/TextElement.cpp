#include "CodeService/FormatElement/TextElement.h"

TextElement::TextElement(std::string_view text, TextRange range)
	: _text(text),
	  FormatElement(range)
{
}

TextElement::TextElement(std::shared_ptr<LuaAstNode> node)
	: TextElement(node->GetText(), node->GetTextRange())
{
}

FormatElementType TextElement::GetType()
{
	return FormatElementType::TextElement;
}

void TextElement::Serialize(FormatContext& ctx, int position, FormatElement& parent)
{
	ctx.Print(_text);
}

std::string_view TextElement::GetText() const
{
	return _text;
}

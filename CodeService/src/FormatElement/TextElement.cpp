#include "CodeService/FormatElement/TextElement.h"

TextElement::TextElement(std::string_view text, TextRange range)
	: Text(text),
	  FormatElement(range)
{
}

FormatElementType TextElement::GetType()
{
	return FormatElementType::TextElement;
}

void TextElement::Serialize(FormatContext& ctx)
{
	ctx.Print(Text);
}

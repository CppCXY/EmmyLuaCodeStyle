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

void TextElement::Diagnosis(DiagnosisContext& ctx, int position, FormatElement& parent)
{
	int character = ctx.GetColumn(_textRange.EndOffset);
	ctx.SetCharacterCount(character);

	if (character > ctx.GetOptions().max_line_length)
	{
		auto line = ctx.GetLine(_textRange.EndOffset);
		ctx.SetLineMaxLength(line, character);
	}
}

std::string_view TextElement::GetText() const
{
	return _text;
}

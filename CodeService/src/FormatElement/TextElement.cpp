#include "CodeService/FormatElement/TextElement.h"
#include "Util/format.h"

TextElement::TextElement(std::string_view text, TextDefineType textDefineType, TextRange range)
	: FormatElement(range),
	  _text(text),
	  _textDefineType(textDefineType)
{
}

TextElement::TextElement(std::shared_ptr<LuaAstNode> node, TextDefineType textDefineType)
	: TextElement(node->GetText(), textDefineType, node->GetTextRange())
{
}

FormatElementType TextElement::GetType()
{
	return FormatElementType::TextElement;
}

void TextElement::Serialize(FormatContext& ctx, int position, FormatElement& parent)
{
	ctx.Print(*this);
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

void TextElement::SetTextDefineType(TextDefineType textDefineType)
{
	_textDefineType = textDefineType;
}

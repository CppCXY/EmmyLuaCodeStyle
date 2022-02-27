#include "CodeService/FormatElement/TextElement.h"
#include "Util/format.h"

TextElement::TextElement(std::string_view text, TextRange range, TextSpaceType space)
	: FormatElement(range),
	  _text(text),
	  _space(TextSpaceType::None)
{
}

TextElement::TextElement(std::shared_ptr<LuaAstNode> node, TextSpaceType space)
	: TextElement(node->GetText(), node->GetTextRange(), space)
{
}

FormatElementType TextElement::GetType()
{
	return FormatElementType::TextElement;
}

void TextElement::SetSpaceType(TextSpaceType type)
{
	_space = type;
}

void TextElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	ctx.Print(_text, GetTextRange());
}

void TextElement::Diagnose(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	const int character = ctx.GetColumn(_textRange.EndOffset);
	ctx.SetCharacterCount(character + 1);

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

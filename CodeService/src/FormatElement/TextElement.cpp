#include "CodeService/FormatElement/TextElement.h"
#include "Util/format.h"

TextElement::TextElement(std::string_view text, TextRange range)
	: FormatElement(range),
	  _text(text),
	  _node(nullptr)
{
}

TextElement::TextElement(std::shared_ptr<LuaAstNode> node)
	: FormatElement(node->GetTextRange()),
	  _node(node),
	  _text(node->GetText())

{
}

FormatElementType TextElement::GetType()
{
	return FormatElementType::TextElement;
}

void TextElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	ctx.Print(_text, GetTextRange());
}

void TextElement::Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
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

std::shared_ptr<LuaAstNode> TextElement::GetNode() const
{
	return _node;
}

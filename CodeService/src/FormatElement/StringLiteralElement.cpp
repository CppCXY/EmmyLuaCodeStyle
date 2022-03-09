#include "CodeService/FormatElement/StringLiteralElement.h"

StringLiteralElement::StringLiteralElement(std::shared_ptr<LuaAstNode> node)
	: FormatElement(node->GetTextRange()),
	  _text(node->GetText())
{
}

FormatElementType StringLiteralElement::GetType()
{
	return FormatElementType::StringLiteralElement;
}

void StringLiteralElement::Serialize(SerializeContext& ctx, ChildIterator, FormatElement& parent)
{
	auto quoteStyle = ctx.GetOptions().quote_style;

	if (quoteStyle != QuoteStyle::None
		&& !_text.empty()
		&& (_text[0] == '\'' || _text[0] == '\"'))
	{
		char del = quoteStyle == QuoteStyle::Double ? '\"' : '\'';

		if (_text.size() >= 2 && !ExistDel(del))
		{
			std::string_view text = _text.substr(1, _text.size() - 2);
			auto range = GetTextRange();

			ctx.Print(del, range.StartOffset);

			ctx.Print(text, TextRange(range.StartOffset + 1, range.EndOffset - 1));

			ctx.Print(del, range.EndOffset);
			return;
		}
	}

	return ctx.Print(_text, GetTextRange());
}

void StringLiteralElement::Diagnosis(DiagnosisContext& ctx, ChildIterator, FormatElement& parent)
{
	//if necessary , add diagnosis
}

std::string_view StringLiteralElement::GetText() const
{
	return _text;
}

bool StringLiteralElement::ExistDel(char del)
{
	auto text = _text.substr(1, _text.size() - 2);
	char ch = '\0';
	for (std::size_t i = 0; i < text.size(); ++i)
	{
		ch = text[i];
		if (ch == del)
		{
			return true;
		}
		else if (ch == '\\')
		{
			++i;
		}
	}

	return false;
}

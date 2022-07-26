#include "CodeService/FormatElement/StatementElement.h"

#include "CodeService/FormatElement/TextElement.h"


StatementElement::StatementElement()
	: _disableFormat(false)
{
}

FormatElementType StatementElement::GetType()
{
	return FormatElementType::StatementElement;
}

void StatementElement::SetDisableFormat()
{
	_disableFormat = true;
}


void StatementElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	if (!_disableFormat) {
		FormatElement::Serialize(ctx, selfIt, parent);
	}
	else
	{
		auto textRange = _textRange;
		std::string_view source = ctx.GetParser()->GetSource();
		std::string_view text = source.substr(textRange.StartOffset, textRange.EndOffset - textRange.StartOffset + 1);

		auto textElement = std::make_shared<TextElement>(text, textRange);
		textElement->Serialize(ctx, selfIt, parent);
	}

	ctx.PrintLine(1);
}

void StatementElement::Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	if (!_disableFormat) {
		FormatElement::Diagnosis(ctx, selfIt, parent);
	}
	ctx.SetCharacterCount(0);
}

#include "CodeService/FormatElement/IndentElement.h"
#include "Util/format.h"
#include "CodeService/FormatElement/FormatElement.h"

IndentElement::IndentElement()
	: FormatElement(),
	  _specialIndent(0),
	  _style(IndentStyle::Space),
	  _defaultIndent(true)
{
}

IndentElement::IndentElement(std::size_t specialIndent, IndentStyle style)
	: FormatElement(),
	  _specialIndent(specialIndent),
	  _style(style),
	  _defaultIndent(false)
{
}

FormatElementType IndentElement::GetType()
{
	return FormatElementType::IndentElement;
}

void IndentElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	if (_defaultIndent)
	{
		ctx.AddIndent();
	}
	else if (_style == IndentStyle::Tab)
	{
		ctx.AddIndent(FormatContext::IndentState{0, _specialIndent, IndentStyle::Tab});
	}
	else
	{
		ctx.AddIndent(FormatContext::IndentState{_specialIndent, 0, IndentStyle::Space});
	}

	FormatElement::Serialize(ctx, selfIt, parent);

	ctx.RecoverIndent();
}

void IndentElement::Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	if (_defaultIndent)
	{
		ctx.AddIndent();
	}
	else if (_style == IndentStyle::Tab)
	{
		ctx.AddIndent(FormatContext::IndentState{0, _specialIndent, IndentStyle::Tab});
	}
	else
	{
		ctx.AddIndent(FormatContext::IndentState{_specialIndent, 0, IndentStyle::Space});
	}
	GeneralIndentDiagnosis(ctx, selfIt, parent);
	ctx.RecoverIndent();
}

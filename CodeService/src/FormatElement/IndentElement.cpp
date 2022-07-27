#include "CodeService/FormatElement/IndentElement.h"
#include "Util/format.h"
#include "CodeService/FormatElement/FormatElement.h"
#include "CodeService/FormatElement/StatementElement.h"
#include "CodeService/FormatElement/TextElement.h"

IndentElement::IndentElement()
	: FormatElement(),
	  _specialIndent(0),
	  _style(IndentStyle::Space),
	  _defaultIndent(true),
	  _formatControl(DisableFormat::None)
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

void IndentElement::EnableDisableNext()
{
	if (_formatControl == DisableFormat::None) {
		_formatControl = DisableFormat::DisableNext;
	}
}

void IndentElement::EnableDisableFormat()
{
	_formatControl = DisableFormat::Disable;
}

bool IndentElement::IsDisableEnv() const
{
	return _formatControl != DisableFormat::None;
}

void IndentElement::AddChild(std::shared_ptr<FormatElement> child)
{
	switch (_formatControl)
	{
	case DisableFormat::None:
		{
			return FormatElement::AddChild(child);
		}
	case DisableFormat::DisableNext:
		{
			if (child->Is(FormatElementType::StatementElement))
			{
				auto statement = std::dynamic_pointer_cast<StatementElement>(child);
				statement->SetDisableFormat();
				_formatControl = DisableFormat::None;
			}
			return FormatElement::AddChild(child);
		}
	case DisableFormat::Disable:
		{
			if (child->Is(FormatElementType::StatementElement))
			{
				auto statement = std::dynamic_pointer_cast<StatementElement>(child);
				statement->SetDisableFormat();
			}
			return FormatElement::AddChild(child);
		}
	}
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

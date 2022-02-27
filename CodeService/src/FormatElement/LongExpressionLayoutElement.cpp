#include "CodeService/FormatElement/LongExpressionLayoutElement.h"
#include "CodeService/FormatElement/KeepElement.h"

LongExpressionLayoutElement::LongExpressionLayoutElement(int continuationIndent)
	: _hasContinuation(false),
	  _continuationIndent(continuationIndent)
{
}

FormatElementType LongExpressionLayoutElement::GetType()
{
	return FormatElementType::LongExpressionLayoutElement;
}

void LongExpressionLayoutElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	SerializeSubExpression(ctx, *this);
	if (_hasContinuation)
	{
		ctx.RecoverIndent();
	}
}

void LongExpressionLayoutElement::Diagnose(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	DiagnosisSubExpression(ctx, *this);
	if (_hasContinuation)
	{
		ctx.RecoverIndent();
	}
}

void LongExpressionLayoutElement::SerializeSubExpression(SerializeContext& ctx, FormatElement& parent)
{
	auto& children = parent.GetChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		auto child = *it;

		if (child->GetType() == FormatElementType::SubExpressionElement)
		{
			SerializeSubExpression(ctx, *child);
		}
		else
		{
			child->Serialize(ctx, it, parent);
		}
		if (child->GetType() == FormatElementType::KeepElement)
		{
			if (ctx.GetCharacterCount() == 0 && !_hasContinuation)
			{
				IndentSubExpression(ctx);
			}
		}
	}
}

void LongExpressionLayoutElement::DiagnosisSubExpression(DiagnosisContext& ctx, FormatElement& parent)
{
	auto& children = parent.GetChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		auto child = *it;

		if (child->GetType() == FormatElementType::SubExpressionElement)
		{
			DiagnosisSubExpression(ctx, *child);
		}
		else
		{
			child->Diagnose(ctx, it, parent);
		}
		if (child->GetType() == FormatElementType::KeepElement)
		{
			if (ctx.GetCharacterCount() == 0 && !_hasContinuation)
			{
				IndentSubExpression(ctx);
			}
		}
	}
}

void LongExpressionLayoutElement::IndentSubExpression(FormatContext& ctx)
{
	_hasContinuation = true;
	if (_continuationIndent != -1) {
		auto& options = ctx.GetOptions();
		auto state = ctx.GetCurrentIndent();
		if (options.indent_style == IndentStyle::Space)
		{
			state.SpaceIndent += _continuationIndent;
		}
		else
		{
			state.TabIndent += _continuationIndent / options.tab_width;
			state.SpaceIndent += _continuationIndent % options.tab_width;
		}
		ctx.AddIndent(state);
	}
	else
	{
		ctx.AddIndent();
	}
}

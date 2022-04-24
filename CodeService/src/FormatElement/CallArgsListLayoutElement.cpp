#include "CodeService/FormatElement/CallArgsListLayoutElement.h"

CallArgsListLayoutElement::CallArgsListLayoutElement()
	: _hasLineBreak(false)
{
}

FormatElementType CallArgsListLayoutElement::GetType()
{
	return FormatElementType::CallArgsListLayoutElement;
}

void CallArgsListLayoutElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	if (_children.empty())
	{
		return;
	}

	if (_children.size() == 1
		&& (_children.front()->Is(FormatElementType::StringLiteralElement)
			|| _children.front()->Is(FormatElementType::ExpressionElement)
		))
	{
		auto node = _children.front();
		node->Serialize(ctx, _children.begin(), *this);
		return;
	}

	for (auto it = _children.begin(); it != _children.end(); ++it)
	{
		if (ctx.GetCharacterCount() == 0 && !_hasLineBreak)
		{
			_hasLineBreak = true;
			ctx.AddIndent();
		}
		auto arg = *it;
		if (arg->Is(FormatElementType::SubExpressionElement))
		{
			SerializeSubExpression(ctx, arg, true);
		}
		else
		{
			arg->Serialize(ctx, it, *this);
		}
	}
	if (_hasLineBreak)
	{
		ctx.RecoverIndent();
	}
}

void CallArgsListLayoutElement::Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	if (_children.empty())
	{
		return;
	}

	if (_children.size() == 1
		&& (_children.front()->Is(FormatElementType::StringLiteralElement)
			|| _children.front()->Is(FormatElementType::ExpressionElement)
			))
	{
		auto node = _children.front();
		node->Diagnosis(ctx, _children.begin(), *this);
		return;
	}

	for (auto it = _children.begin(); it != _children.end(); ++it)
	{
		if (ctx.GetCharacterCount() == 0 && !_hasLineBreak)
		{
			_hasLineBreak = true;
			ctx.AddIndent();
		}
		auto arg = *it;
		if (arg->Is(FormatElementType::SubExpressionElement))
		{
			DiagnoseSubExpression(ctx, arg, true);
		}
		else
		{
			arg->Diagnosis(ctx, it, *this);
		}
	}
	if (_hasLineBreak)
	{
		ctx.RecoverIndent();
	}
}

void CallArgsListLayoutElement::SerializeSubExpression(SerializeContext& ctx, std::shared_ptr<FormatElement> parent,
                                                       bool topLevelSubexpression)
{
	if (parent->GetChildren().empty())
	{
		return;
	}
	auto& children = parent->GetChildren();
	auto it = children.begin();

	if (topLevelSubexpression && _hasLineBreak && IsLastArg(parent))
	{
		auto expression = *it;
		if (expression->Is(FormatElementType::ExpressionElement))
		{
			auto textRange = expression->GetTextRange();
			// 如果跨行
			auto startLine = ctx.GetLine(textRange.StartOffset);
			if (startLine != ctx.GetLine(textRange.EndOffset))
			{
				ctx.PrintIndentOnly(startLine);
				_hasLineBreak = false;
				ctx.RecoverIndent();
			}
		}
		else if (expression->Is(FormatElementType::StringLiteralElement) && IsFirstArg(parent))
		{
			_hasLineBreak = false;
			ctx.RecoverIndent();
		}
	}

	for (; it != children.end(); ++it)
	{
		auto child = *it;

		if (child->Is(FormatElementType::SubExpressionElement))
		{
			SerializeSubExpression(ctx, child, false);
		}
		else
		{
			child->Serialize(ctx, it, *parent);
		}
		if (child->Is(FormatElementType::KeepElement))
		{
			if (ctx.GetCharacterCount() == 0 && !_hasLineBreak)
			{
				_hasLineBreak = true;
				ctx.AddIndent();
			}
		}
	}
}

void CallArgsListLayoutElement::DiagnoseSubExpression(DiagnosisContext& ctx, std::shared_ptr<FormatElement> parent,
                                                      bool topLevelSubexpression)
{
	auto& children = parent->GetChildren();
	if (parent->GetChildren().empty())
	{
		return;
	}
	auto it = children.begin();

	if (topLevelSubexpression && _hasLineBreak && IsLastArg(parent))
	{
		auto expression = *it;
		if (expression->Is(FormatElementType::ExpressionElement))
		{
			auto textRange = expression->GetTextRange();
			// 如果跨行
			auto startLine = ctx.GetLine(textRange.StartOffset);
			if (startLine != ctx.GetLine(textRange.EndOffset))
			{
				_hasLineBreak = false;
				ctx.RecoverIndent();
			}
		}
		else if (expression->Is(FormatElementType::StringLiteralElement) && IsFirstArg(parent))
		{
			_hasLineBreak = false;
			ctx.RecoverIndent();
		}
	}

	for (; it != children.end(); ++it)
	{
		auto child = *it;

		if (child->Is(FormatElementType::SubExpressionElement))
		{
			DiagnoseSubExpression(ctx, child, false);
		}
		else
		{
			child->Diagnosis(ctx, it, *parent);
		}
		if (child->Is(FormatElementType::KeepElement))
		{
			if (ctx.GetCharacterCount() == 0 && !_hasLineBreak)
			{
				// IndentSubExpression(ctx);
				ctx.AddIndent();
				_hasLineBreak = true;
			}
		}
	}
}

bool CallArgsListLayoutElement::IsLastArg(std::shared_ptr<FormatElement> node)
{
	for (auto it = _children.rbegin(); it != _children.rend(); ++it)
	{
		if ((*it)->HasValidTextRange())
		{
			return *it == node;
		}
	}

	return false;
}

bool CallArgsListLayoutElement::IsFirstArg(std::shared_ptr<FormatElement> node)
{
	for (auto it = _children.begin(); it != _children.end(); ++it)
	{
		if ((*it)->HasValidTextRange())
		{
			return *it == node;
		}
	}

	return false;
}

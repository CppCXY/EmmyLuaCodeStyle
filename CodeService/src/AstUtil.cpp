#include "CodeService/AstUtil.h"

bool ast_util::IsSingleStringOrTableArg(std::shared_ptr<LuaAstNode> callArgList)
{
	if (!callArgList)
	{
		return false;
	}

	for (auto child : callArgList->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::StringLiteralExpression:
		case LuaAstNodeType::TableExpression:
			{
				return true;
			}
		case LuaAstNodeType::ExpressionList:
			{
				auto expressionList = child;
				if (expressionList->GetChildren().empty()
					|| expressionList->FindFirstOf(LuaAstNodeType::GeneralOperator))
				{
					return false;
				}

				auto expression = expressionList->GetChildren().front();
				if (expression->FindFirstOf(LuaAstNodeType::StringLiteralExpression)
					|| expression->FindFirstOf(LuaAstNodeType::TableExpression))
				{
					return true;
				}
				break;
			}
		default:
			{
				break;
			}
		}
	}

	return false;
}

bool ast_util::IsSingleStringArg(std::shared_ptr<LuaAstNode> callArgList)
{
	if (!callArgList)
	{
		return false;
	}

	for (auto child : callArgList->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::StringLiteralExpression:
			{
				return true;
			}
		case LuaAstNodeType::ExpressionList:
			{
				auto expressionList = child;
				if (expressionList->GetChildren().empty()
					|| expressionList->FindFirstOf(LuaAstNodeType::GeneralOperator))
				{
					return false;
				}

				auto expression = expressionList->GetChildren().front();
				if (expression->FindFirstOf(LuaAstNodeType::StringLiteralExpression))
				{
					return true;
				}
				break;
			}
		default:
			{
				break;
			}
		}
	}

	return false;
}

bool ast_util::IsSingleTableArg(std::shared_ptr<LuaAstNode> callArgList)
{
	if (!callArgList)
	{
		return false;
	}

	for (auto child : callArgList->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::TableExpression:
			{
				return true;
			}
		case LuaAstNodeType::ExpressionList:
			{
				auto expressionList = child;
				if (expressionList->GetChildren().empty()
					|| expressionList->FindFirstOf(LuaAstNodeType::GeneralOperator))
				{
					return false;
				}

				auto expression = expressionList->GetChildren().front();
				if (expression->FindFirstOf(LuaAstNodeType::TableExpression))
				{
					return true;
				}
				break;
			}
		default:
			{
				break;
			}
		}
	}

	return false;
}

bool ast_util::WillCallArgHaveParentheses(std::shared_ptr<LuaAstNode> callArgList,
                                          CallArgParentheses callArgParentheses)
{
	if (ast_util::IsSingleStringOrTableArg(callArgList))
	{
		if (IsSingleStringArg(callArgList) && callArgParentheses == CallArgParentheses::RemoveStringLiteralOnly)
		{
			return false;
		}
		else if (IsSingleTableArg(callArgList) && callArgParentheses == CallArgParentheses::RemoveTableOnly)
		{
			return false;
		}
		else if (callArgParentheses == CallArgParentheses::Remove)
		{
			return false;
		}
		else
		{
			auto leftParentheses = callArgList->FindFirstOf(LuaAstNodeType::GeneralOperator);
			if (leftParentheses && leftParentheses->GetText() == "(")
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}

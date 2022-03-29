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


bool ast_util::IsSingleStringArgUnambiguous(std::shared_ptr<LuaAstNode> callArgList)
{
	bool isSingleStringArg = IsSingleStringArg(callArgList);
	if (!isSingleStringArg)
	{
		return false;
	}

	auto callExpression = callArgList->GetParent();
	if (!callExpression)
	{
		return false;
	}
	if (callExpression->GetType() != LuaAstNodeType::CallExpression)
	{
		return false;
	}

	auto indexExpression = callExpression->GetParent();
	if (indexExpression && indexExpression->GetType() == LuaAstNodeType::IndexExpression)
	{
		if (!indexExpression->GetChildren().empty() && callExpression == indexExpression->GetChildren().front())
		{
			return false;
		}
	}

	return true;
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
		switch (callArgParentheses)
		{
		case CallArgParentheses::Remove:
			{
				return false;
			}
		case CallArgParentheses::RemoveStringOnly:
			{
				if (IsSingleStringArg(callArgList))
				{
					return false;
				}
				break;
			}
		case CallArgParentheses::RemoveTableOnly:
			{
				if (IsSingleTableArg(callArgList))
				{
					return false;
				}
				break;
			}
		case CallArgParentheses::UnambiguousRemoveStringOnly:
			{
				if (IsSingleStringArgUnambiguous(callArgList))
				{
					return false;
				}
				break;
			}
		default:
			{
				break;
			}
		}

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

	return true;
}

std::shared_ptr<LuaAstNode> ast_util::FindLeftIndexExpression(std::shared_ptr<LuaAstNode> expression)
{
	if (expression->GetChildren().empty())
	{
		return nullptr;
	}

	std::shared_ptr<LuaAstNode> leftIndex = nullptr;
	auto subExpression = expression->GetChildren().front();
	while (subExpression)
	{
		switch (subExpression->GetType())
		{
		case LuaAstNodeType::CallExpression:
			{
				if (subExpression->GetChildren().empty())
				{
					subExpression = nullptr;
				}
				else
				{
					subExpression = subExpression->GetChildren().front();
				}
				break;
			}
		case LuaAstNodeType::IndexExpression:
			{
				leftIndex = subExpression;
				if (subExpression->GetChildren().empty())
				{
					subExpression = nullptr;
				}
				else
				{
					subExpression = subExpression->GetChildren().front();
				}
				break;
			}
		case LuaAstNodeType::PrimaryExpression:
			{
				subExpression = nullptr;
				break;
			}
		default:
			{
				return nullptr;
			}
		}
	}

	return leftIndex;
}

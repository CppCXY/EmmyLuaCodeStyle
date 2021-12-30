#include "LuaParser/LuaAstVisitor.h"

LuaAstVisitor::~LuaAstVisitor()
{
}

void LuaAstVisitor::Visit(std::shared_ptr<LuaAstNode> node)
{
	switch (node->GetType())
	{
	case LuaAstNodeType::Block:
		{
			VisitBlock(node);
			break;
		}
	case LuaAstNodeType::LocalStatement:
		{
			VisitLocalStatement(node);
			break;
		}
	case LuaAstNodeType::NameDefList:
		{
			VisitNameDefList(node);
			break;
		}
	case LuaAstNodeType::ExpressionList:
		{
			VisitExpressionList(node);
			break;
		}
	case LuaAstNodeType::Comment:
		{
			VisitComment(node);
			break;
		}
	case LuaAstNodeType::LabelStatement:
		{
			VisitLabelStatement(node);
			break;
		}
	case LuaAstNodeType::BreakStatement:
		{
			VisitBreakStatement(node);
			break;
		}
	case LuaAstNodeType::GotoStatement:
		{
			VisitGotoStatement(node);
			break;
		}
	case LuaAstNodeType::ReturnStatement:
		{
			VisitReturnStatement(node);
			break;
		}
	case LuaAstNodeType::Identify:
		{
			VisitIdentify(node);
			break;
		}
	case LuaAstNodeType::NameIdentify:
		{
			VisitNameIdentify(node);
			break;
		}
	case LuaAstNodeType::GeneralOperator:
		{
			VisitGeneralOperator(node);
			break;
		}
	case LuaAstNodeType::KeyWord:
		{
			VisitKeyWords(node);
			break;
		}
	case LuaAstNodeType::WhileStatement:
		{
			VisitWhileStatement(node);
			break;
		}
	case LuaAstNodeType::ForStatement:
		{
			VisitForStatement(node);
			break;
		}
	case LuaAstNodeType::ForBody:
		{
			VisitForBody(node);
			break;
		}
	case LuaAstNodeType::Attribute:
		{
			VisitAttribute(node);
			break;
		}
	case LuaAstNodeType::RepeatStatement:
		{
			VisitRepeatStatement(node);
			break;
		}
	case LuaAstNodeType::DoStatement:
		{
			VisitDoStatement(node);
			break;
		}
	case LuaAstNodeType::AssignStatement:
		{
			VisitAssignment(node);
			break;
		}
	case LuaAstNodeType::IfStatement:
		{
			VisitIfStatement(node);
			break;
		}
	case LuaAstNodeType::ExpressionStatement:
		{
			VisitExpressionStatement(node);
			break;
		}
	case LuaAstNodeType::Expression:
		{
			VisitExpression(node);
			break;
		}
	case LuaAstNodeType::CallExpression:
		{
			VisitCallExpression(node);
			break;
		}
	case LuaAstNodeType::CallArgList:
		{
			VisitCallArgList(node);
			break;
		}
	case LuaAstNodeType::FunctionStatement:
		{
			VisitFunctionStatement(node);
			break;
		}
	case LuaAstNodeType::LocalFunctionStatement:
		{
			VisitLocalFunctionStatement(node);
			break;
		}
	case LuaAstNodeType::ClosureExpression:
		{
			VisitClosureExpression(node);
			break;
		}
	case LuaAstNodeType::NameExpression:
		{
			VisitNameExpression(node);
			break;
		}
	case LuaAstNodeType::TableExpression:
		{
			VisitTableExpression(node);
			break;
		}
	case LuaAstNodeType::TableField:
		{
			VisitTableField(node);
			break;
		}
	case LuaAstNodeType::ParamList:
		{
			VisitParamList(node);
			break;
		}
	case LuaAstNodeType::FunctionBody:
		{
			VisitFunctionBody(node);
			break;
		}
	case LuaAstNodeType::LiteralExpression:
		{
			VisitLiteralExpression(node);
			break;
		}
	default:
		{
			break;
		}
	}

	for (const auto& child : node->GetChildren())
	{
		Visit(child);
	}
}

void LuaAstVisitor::VisitBlock(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitLocalStatement(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitNameDefList(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitExpressionList(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitComment(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitLabelStatement(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitBreakStatement(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitGotoStatement(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitReturnStatement(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitIdentify(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitNameIdentify(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitGeneralOperator(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitKeyWords(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitWhileStatement(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitForStatement(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitForBody(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitAttribute(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitRepeatStatement(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitDoStatement(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitAssignment(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitIfStatement(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitExpressionStatement(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitExpression(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitCallExpression(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitCallArgList(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitFunctionStatement(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitLocalFunctionStatement(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitClosureExpression(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitNameExpression(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitTableExpression(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitTableField(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitParamList(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitFunctionBody(const std::shared_ptr<LuaAstNode>& node)
{
}

void LuaAstVisitor::VisitLiteralExpression(const std::shared_ptr<LuaAstNode>& node)
{
}

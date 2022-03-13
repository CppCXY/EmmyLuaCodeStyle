#include "CodeService/LuaLinter.h"

#include "CodeService/AstUtil.h"
#include "CodeService/FormatElement/AlignIfLayoutElement.h"
#include "CodeService/FormatElement/AlignmentLayoutElement.h"
#include "CodeService/FormatElement/ExpressionElement.h"
#include "CodeService/FormatElement/IndentElement.h"
#include "CodeService/FormatElement/KeepLineElement.h"
#include "CodeService/FormatElement/LongExpressionLayoutElement.h"
#include "CodeService/FormatElement/NoIndentElement.h"
#include "CodeService/FormatElement/StatementElement.h"
#include "CodeService/FormatElement/SubExpressionElement.h"
#include "CodeService/FormatElement/TextElement.h"
#include "CodeService/NameStyle/NameStyleChecker.h"


LuaLinter::LuaLinter(std::shared_ptr<LuaParser> luaParser, LuaLinterOptions& options)
	: _parser(luaParser),
	  _options(options),
	  _env(nullptr)
{
}

void LuaLinter::BuildDiagnosisElement()
{
	auto chunkNode = _parser->GetAst();
	if (!chunkNode->GetChildren().empty())
	{
		_env = DiagnoseBlock(chunkNode->GetChildren().front());
	}
	else
	{
		_env = std::make_shared<IndentElement>();
	}
}

std::vector<LuaDiagnosisInfo> LuaLinter::GetDiagnoseResult()
{
	DiagnosisContext ctx(_parser, _options);

	_env->DiagnoseCodeStyle(ctx);

	if (_options.name_style)
	{
		NameStyleChecker checker(ctx);
		auto chunkAst = _parser->GetAst();
		chunkAst->AcceptChildren(checker);
		checker.Analysis();
	}

	return ctx.GetDiagnosisInfos();
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseNode(std::shared_ptr<LuaAstNode> block)
{
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseBlock(std::shared_ptr<LuaAstNode> block)
{
	auto indentEnv = std::make_shared<IndentElement>();

	auto& statements = block->GetChildren();

	for (auto it = statements.begin(); it != statements.end(); ++it)
	{
		const auto statement = *it;
		switch (statement->GetType())
		{
		case LuaAstNodeType::AssignStatement:
		case LuaAstNodeType::LocalStatement:
			{
				if (ast_util::NextMatch(it, LuaAstNodeType::AssignStatement, statements) ||
					ast_util::NextMatch(it, LuaAstNodeType::LocalStatement, statements) ||
					ast_util::NextMatch(it, LuaAstNodeType::Comment, statements))
				{
					indentEnv->AddChild(DiagnoseAlignStatement(it, statements));
				}
				else
				{
					auto childEnv = DiagnoseNode(statement);
					indentEnv->AddChild(childEnv);
				}

				if (_options.line_space.keep_line_after_local_or_assign_statement)
				{
					indentEnv->AddChild(_options.line_space.keep_line_after_local_or_assign_statement.Element);
				}
				break;
			}
		case LuaAstNodeType::RepeatStatement:
			{
				indentEnv->AddChild(DiagnoseRepeatStatement(statement));
				if (_options.line_space.keep_line_after_repeat_statement)
				{
					indentEnv->AddChild(_options.line_space.keep_line_after_repeat_statement.Element);
				}
				break;
			}
		case LuaAstNodeType::DoStatement:
			{
				indentEnv->AddChild(DiagnoseDoStatement(statement));
				if (_options.line_space.keep_line_after_do_statement)
				{
					indentEnv->AddChild(_options.line_space.keep_line_after_do_statement.Element);
				}
				break;
			}
		case LuaAstNodeType::ForStatement:
			{
				indentEnv->AddChild(DiagnoseIfStatement(statement));
				if (_options.line_space.keep_line_after_for_statement)
				{
					indentEnv->AddChild(_options.line_space.keep_line_after_for_statement.Element);
				}
				break;
			}
		case LuaAstNodeType::WhileStatement:
			{
				indentEnv->AddChild(DiagnoseWhileStatement(statement));
				if (_options.line_space.keep_line_after_while_statement)
				{
					indentEnv->AddChild(_options.line_space.keep_line_after_while_statement.Element);
				}
				break;
			}
		case LuaAstNodeType::IfStatement:
			{
				indentEnv->AddChild(DiagnoseNode(statement));
				if (_options.line_space.keep_line_after_if_statement)
				{
					indentEnv->AddChild(_options.line_space.keep_line_after_if_statement.Element);
				}
				break;
			}
		case LuaAstNodeType::Comment:
			{
				auto comment = DiagnoseNode(statement);
				auto commentStatement = std::make_shared<StatementElement>();
				commentStatement->AddChild(comment);
				indentEnv->AddChild(commentStatement);
				break;
			}
		case LuaAstNodeType::BreakStatement:
		case LuaAstNodeType::ReturnStatement:
		case LuaAstNodeType::GotoStatement:
		case LuaAstNodeType::ExpressionStatement:
			{
				auto statEnv = DiagnoseNode(statement);
				if (ast_util::NextMatch(it, LuaAstNodeType::Comment, statements))
				{
					auto next = ast_util::NextNode(it, statements);
					int currentLine = _parser->GetLine(statement->GetTextRange().EndOffset);
					int nextLine = _parser->GetLine(next->GetTextRange().StartOffset);

					if (currentLine == nextLine)
					{
						// statEnv->Add<KeepBlankElement>(1);
						statEnv->Add<TextElement>(next);
						++it;
					}
				}
				indentEnv->AddChild(statEnv);
				break;
			}
		case LuaAstNodeType::LocalFunctionStatement:
		case LuaAstNodeType::FunctionStatement:
			{
				indentEnv->AddChild(DiagnoseNode(statement));
				if (_options.line_space.keep_line_after_function_define_statement)
				{
					indentEnv->AddChild(_options.line_space.keep_line_after_function_define_statement.Element);
				}
				break;
			}
		case LuaAstNodeType::LabelStatement:
			{
				auto childEnv = DiagnoseLabelStatement(statement);
				childEnv->SetDescription(LinterDescription::no_linter);
				indentEnv->AddChild(childEnv);
				break;
			}
		default:
			{
				indentEnv->AddChild(DiagnoseNode(statement));
			}
		}
	}

	return indentEnv;
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseLocalStatement(std::shared_ptr<LuaAstNode> localStatement)
{
	auto env = std::make_shared<StatementElement>();
	for (auto& node : localStatement->GetChildren())
	{
		switch (node->GetType())
		{
		case LuaAstNodeType::KeyWord:
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<TextElement>(node, _options.white_space.CalculateSpace(node));

				break;
			}
		case LuaAstNodeType::NameDefList:
			{
				env->AddChild(DiagnoseNode(node));
				break;
			}
		case LuaAstNodeType::ExpressionList:
			{
				env->AddChild(DiagnoseExpressionList(
					node,
					std::make_shared<LongExpressionLayoutElement>(
						_options.indent_style.ContinuationIndent)));
				break;
			}
		case LuaAstNodeType::Comment:
			{
				env->AddChild(DiagnoseNode(node));
				break;
			}
		default:
			DefaultHandle(node, env);
			break;
		}
	}

	return env;
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseAssignmentStatement(std::shared_ptr<LuaAstNode> assignStatement)
{
	auto env = std::make_shared<StatementElement>();
	for (auto& child : assignStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<TextElement>(child);
				break;
			}
		case LuaAstNodeType::ExpressionList:
			{
				env->AddChild(DiagnoseExpressionList(
					child,
					std::make_shared<LongExpressionLayoutElement>(
						_options.indent_style.ContinuationIndent)));
				break;
			}
		case LuaAstNodeType::Comment:
			{
				env->Add<TextElement>(child);
				break;
			}
		default:
			DefaultHandle(child, env);
			break;
		}
	}

	return env;
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseRepeatStatement(std::shared_ptr<LuaAstNode> repeatStatement)
{
	auto env = std::make_shared<StatementElement>();
	auto& children = repeatStatement->GetChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		const auto child = *it;
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				if (child->GetText() == "repeat")
				{
					env->AddChild(DiagnoseNodeAndBlockOrEnd(it, children));
				}
				else
				{
					env->Add<TextElement>(child, _options.white_space.CalculateSpace(child));
				}
				break;
			}

		default:
			{
				DefaultHandle(child, env);
				break;
			}
		}
	}

	return env;
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseDoStatement(std::shared_ptr<LuaAstNode> doStatement)
{
	std::shared_ptr<FormatElement> env = std::make_shared<StatementElement>();
	auto& children = doStatement->GetChildren();
	auto it = children.begin();
	auto doBlockEnd = DiagnoseNodeAndBlockOrEnd(it, children);
	if (_options.indent_style.NoIndent)
	{
		for (auto& child : doBlockEnd->GetChildren())
		{
			if (child->GetType() == FormatElementType::IndentElement)
			{
				auto indentElement = std::make_shared<IndentElement>();
				auto noIndent = std::make_shared<NoIndentElement>();
				noIndent->AddChildren(child->GetChildren());
				indentElement->AddChild(noIndent);
				child = indentElement;
			}
		}
	}

	env->AddChild(doBlockEnd);
	return env;
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseIfStatement(std::shared_ptr<LuaAstNode> ifStatement)
{
	auto env = std::make_shared<StatementElement>();
	auto& children = ifStatement->GetChildren();

	for (auto it = children.begin(); it != children.end(); ++it)
	{
		const auto child = *it;
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				if (child->GetText() == "then" || child->GetText() == "else")
				{
					env->AddChild(DiagnoseNodeAndBlockOrEnd(it, children));
				}
				else if (child->GetText() == "if" || child->GetText() == "elseif")
				{
					env->Add<TextElement>(child, _options.white_space.CalculateSpace(child));
				}
				else
				{
					env->Add<TextElement>(child);
				}

				break;
			}
		case LuaAstNodeType::Expression:
			{
				std::shared_ptr<FormatElement> expression = nullptr;
				if (_options.indent_style.NoIndent.IfCondition
					|| _options.align_style.IfCondition != AlignStyleLevel::Ban)
				{
					expression = std::make_shared<LongExpressionLayoutElement>(0);
				}

				env->AddChild(DiagnoseExpression(child, expression));
				break;
			}
		default:
			{
				DefaultHandle(child, env);
				break;
			}
		}
	}

	if (_options.align_style.IfCondition != AlignStyleLevel::Ban)
	{
		auto ifAlignLayout = std::make_shared<AlignIfElement>();
		ifAlignLayout->CopyFrom(env);
		env->Reset();
		env->AddChild(ifAlignLayout);
	}

	return env;
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseForStatement(std::shared_ptr<LuaAstNode> forStatement)
{
	auto env = std::make_shared<StatementElement>();
	for (auto& child : forStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->Add<TextElement>(child, _options.white_space.CalculateSpace(child));
				break;
			}
		case LuaAstNodeType::ForNumber:
			{
				for (auto& forNumberChild : child->GetChildren())
				{
					switch (forNumberChild->GetType())
					{
					case LuaAstNodeType::Identify:
					case LuaAstNodeType::GeneralOperator:
						{
							env->Add<TextElement>(forNumberChild, _options.white_space.CalculateSpace(forNumberChild));
							break;
						}
					case LuaAstNodeType::Expression:
						{
							env->AddChild(DiagnoseExpression(forNumberChild));
							break;
						}
					case LuaAstNodeType::ForBody:
						{
							env->AddChild(DiagnoseForBody(forNumberChild));
							break;
						}
					default:
						{
							DefaultHandle(forNumberChild, env);
							break;
						}
					}
				}
				break;
			}
		case LuaAstNodeType::ForList:
			{
				for (auto& forListChild : child->GetChildren())
				{
					switch (forListChild->GetType())
					{
					case LuaAstNodeType::KeyWord:
						{
							env->Add<TextElement>(forListChild, _options.white_space.CalculateSpace(forListChild));
							break;
						}
					case LuaAstNodeType::NameDefList:
					case LuaAstNodeType::ExpressionList:
						{
							env->AddChild(DiagnoseNode(forListChild));
							break;
						}
					default:
						{
							DefaultHandle(forListChild, env);
							break;
						}
					}
				}
				break;
			}
		default:
			{
				DefaultHandle(child, env);
				break;
			}
		}
	}
	return env;
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseForBody(std::shared_ptr<LuaAstNode> forBody)
{
	auto env = std::make_shared<ExpressionElement>();
	auto& children = forBody->GetChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		const auto child = *it;
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				if (child->GetText() == "do")
				{
					env->AddChild(DiagnoseNodeAndBlockOrEnd(it, children));
				}
				else
				{
					env->Add<TextElement>(child, _options.white_space.CalculateSpace(child));
				}
				break;
			}
		case LuaAstNodeType::Block:
			{
				env->AddChild(DiagnoseBlock(child));
				break;
			}
		default:
			{
				DefaultHandle(child, env);
				break;
			}
		}
	}

	return env;
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseWhileStatement(std::shared_ptr<LuaAstNode> whileStatement)
{
	auto env = std::make_shared<StatementElement>();
	auto& children = whileStatement->GetChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		const auto child = *it;
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				if (child->GetText() == "do")
				{
					env->AddChild(DiagnoseNodeAndBlockOrEnd(it, children));
				}
				else
				{
					env->Add<TextElement>(child, _options.white_space.CalculateSpace(child));
				}
				break;
			}
		case LuaAstNodeType::Expression:
			{
				env->AddChild(DiagnoseExpression(child));
				break;
			}
		default:
			{
				DefaultHandle(child, env);
				break;
			}
		}
	}
	return env;
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseBreakStatement(std::shared_ptr<LuaAstNode> breakStatement)
{
	auto env = std::make_shared<StatementElement>();

	for (auto& child : breakStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<TextElement>(child, _options.white_space.CalculateSpace(child));
				break;
			}
		default:
			DefaultHandle(child, env);
			break;
		}
	}
	return env;
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseReturnStatement(std::shared_ptr<LuaAstNode> returnStatement)
{
	auto env = std::make_shared<StatementElement>();

	for (auto& child : returnStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<TextElement>(child, _options.white_space.CalculateSpace(child));
				break;
			}
		case LuaAstNodeType::ExpressionList:
			{
				env->AddChild(DiagnoseExpressionList(
					child,
					std::make_shared<LongExpressionLayoutElement>(
						_options.indent_style.ContinuationIndent)));
				break;
			}
		case LuaAstNodeType::Comment:
			{
				env->Add<TextElement>(child);
				break;
			}
		default:
			DefaultHandle(child, env);
			break;
		}
	}
	return env;
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseGotoStatement(std::shared_ptr<LuaAstNode> gotoStatement)
{
	auto env = std::make_shared<StatementElement>();

	for (auto& child : gotoStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<TextElement>(child);
				break;
			}
		case LuaAstNodeType::Identify:
			{
				env->Add<TextElement>(child);
				break;
			}

		default:
			DefaultHandle(child, env);
			break;
		}
	}
	return env;
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseExpressionStatement(std::shared_ptr<LuaAstNode> expressionStatement)
{
	auto env = std::make_shared<StatementElement>();

	for (auto& child : expressionStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::CallExpression:
			{
				env->AddChild(DiagnoseCallExpression(child));
				break;
			}
		case LuaAstNodeType::Expression:
			{
				env->AddChild(DiagnoseExpression(child));
				break;
			}
		default:
			{
				DefaultHandle(child, env);
			}
		}
	}

	return env;
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseFunctionStatement(std::shared_ptr<LuaAstNode> functionStatement)
{
	auto env = std::make_shared<StatementElement>();

	for (auto& child : functionStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->Add<TextElement>(child, _options.white_space.CalculateSpace(child));
				break;
			}
		case LuaAstNodeType::NameExpression:
			{
				env->AddChild(DiagnoseNameExpression(child));
				break;
			}
		case LuaAstNodeType::FunctionBody:
			{
				env->AddChild(DiagnoseFunctionBody(child));
				break;
			}
		default:
			{
				DefaultHandle(child, env);
			}
		}
	}
	return env;
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseLocalFunctionStatement(
	std::shared_ptr<LuaAstNode> localFunctionStatement)
{
	auto env = std::make_shared<StatementElement>();

	for (auto& child : localFunctionStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->Add<TextElement>(child, _options.white_space.CalculateSpace(child));
				break;
			}
		case LuaAstNodeType::Identify:
			{
				env->Add<TextElement>(child);
				break;
			}
		case LuaAstNodeType::FunctionBody:
			{
				env->AddChild(DiagnoseFunctionBody(child));
				break;
			}
		default:
			{
				DefaultHandle(child, env);
			}
		}
	}
	return env;
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseFunctionBody(std::shared_ptr<LuaAstNode> functionBody)
{
	auto& children = functionBody->GetChildren();
	auto it = children.begin();
	return DiagnoseNodeAndBlockOrEnd(it, children);
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseNameExpression(std::shared_ptr<LuaAstNode> nameExpression)
{
	auto env = std::make_shared<ExpressionElement>();

	for (auto& child : nameExpression->GetChildren())
	{
		DiagnoseSubExpression(child, env);
	}

	return env;
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseLabelStatement(std::shared_ptr<LuaAstNode> labelStatement)
{
	auto env = std::make_shared<StatementElement>();

	for (auto child : labelStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<TextElement>(child);
				break;
			}
		case LuaAstNodeType::Identify:
			{
				env->Add<TextElement>(child);
				break;
			}

		default:
			DefaultHandle(child, env);
			break;
		}
	}
	return env;
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseParamList(std::shared_ptr<LuaAstNode> paramList)
{
	auto env = std::make_shared<ExpressionElement>();

	auto& children = paramList->GetChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		const auto child = *it;
		switch (child->GetType())
		{
		case LuaAstNodeType::GeneralOperator:
			{
				if (child->GetText() == ",")
				{
					env->Add<TextElement>(child, _options.white_space.CalculateSpace(child));
				}
				else
				{
					env->Add<TextElement>(child);
				}
				break;
			}
		case LuaAstNodeType::Param:
			{
				env->Add<TextElement>(child);
				break;
			}
		case LuaAstNodeType::Comment:
			{
				env->Add<TextElement>(child);
				break;
			}
		default:
			{
				DefaultHandle(child, env);
			}
		}
	}
	return env;
}


std::shared_ptr<FormatElement> LuaLinter::DiagnoseExpression(std::shared_ptr<LuaAstNode> expression,
                                                             std::shared_ptr<FormatElement> env)
{
	if (env == nullptr)
	{
		env = std::make_shared<LongExpressionLayoutElement>(_options.indent_style.ContinuationIndent);
	}

	auto& children = expression->GetChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		const auto current = *it;

		DiagnoseSubExpression(current, env);
	}

	return env;
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseExpressionList(std::shared_ptr<LuaAstNode> expressionList,
                                                                 std::shared_ptr<FormatElement> env)
{
	if (env == nullptr)
	{
		env = std::make_shared<LongExpressionLayoutElement>(_options.indent_style.ContinuationIndent);
	}

	for (auto& node : expressionList->GetChildren())
	{
		switch (node->GetType())
		{
		case LuaAstNodeType::Expression:
			{
				auto subEnv = std::make_shared<SubExpressionElement>();
				env->AddChild(DiagnoseExpression(node, subEnv));
				break;
			}
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<TextElement>(node, _options.white_space.CalculateSpace(node));
				break;
			}
		default:
			DefaultHandle(node, env);
		}
	}
}

void LuaLinter::DiagnoseSubExpression(std::shared_ptr<LuaAstNode> expression, std::shared_ptr<FormatElement> env)
{
	switch (expression->GetType())
	{
	case LuaAstNodeType::BinaryExpression:
		{
			env->AddChild(DiagnoseBinaryExpression(expression));
			break;
		}
	case LuaAstNodeType::UnaryExpression:
		{
			env->AddChild(DiagnoseUnaryExpression(expression));
			break;
		}
	case LuaAstNodeType::IndexExpression:
		{
			env->AddChild(DiagnoseIndexExpression(expression));
			break;
		}
	case LuaAstNodeType::PrimaryExpression:
		{
			env->AddChild(DiagnosePrimaryExpression(expression));
			break;
		}
	case LuaAstNodeType::CallExpression:
		{
			env->AddChild(DiagnoseCallExpression(expression));
			break;
		}
	default:
		{
			DefaultHandle(expression, env);
		}
	}
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseBinaryExpression(std::shared_ptr<LuaAstNode> binaryExpression)
{
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseUnaryExpression(std::shared_ptr<LuaAstNode> unaryExpression)
{
}

std::shared_ptr<FormatElement> LuaLinter::DiagnosePrimaryExpression(std::shared_ptr<LuaAstNode> primaryExpression)
{
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseIndexExpression(std::shared_ptr<LuaAstNode> indexExpression)
{
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseCallExpression(std::shared_ptr<LuaAstNode> callExpression)
{
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseTableAppendExpression(std::shared_ptr<LuaAstNode> expression)
{
}


std::shared_ptr<FormatElement> LuaLinter::DiagnoseAlignStatement(LuaAstNode::ChildIterator& it,
                                                                 const LuaAstNode::ChildrenContainer& children)
{
	std::shared_ptr<FormatElement> env = nullptr;
	if (_options.align_style.LocalOrAssign != AlignStyleLevel::Ban)
	{
		env = std::make_shared<AlignmentLayoutElement>("=", _options.align_style.LocalOrAssign);
	}
	else
	{
		env = std::make_shared<ExpressionElement>();
	}

	env->AddChild(DiagnoseNode(*it));

	auto nextChild = ast_util::NextNode(it, children);
	if (nextChild == nullptr)
	{
		return env->GetChildren().front();
	}

	while (nextChild->GetType() == LuaAstNodeType::AssignStatement
		|| nextChild->GetType() == LuaAstNodeType::LocalStatement
		|| nextChild->GetType() == LuaAstNodeType::Comment)
	{
		auto currentChild = *it;
		int currentLine = _parser->GetLine(currentChild->GetTextRange().EndOffset);
		int nextLine = _parser->GetLine(nextChild->GetTextRange().StartOffset);
		if (nextLine - currentLine > _options.align_style.MaxContinuousLineDistance)
		{
			break;
		}

		if ((currentChild->GetType() == LuaAstNodeType::LocalStatement
				|| currentChild->GetType() == LuaAstNodeType::AssignStatement)
			&& nextChild->GetType() == LuaAstNodeType::Comment && nextLine == currentLine)
		{
			auto lastStatementEnv = env->LastValidElement();
			if (lastStatementEnv)
			{
				lastStatementEnv->Add<TextElement>(nextChild);
			}
		}
		else
		{
			if ((currentChild->GetType() == LuaAstNodeType::LocalStatement
					|| currentChild->GetType() == LuaAstNodeType::AssignStatement)
				&& _options.line_space.keep_line_after_local_or_assign_statement)
			{
				env->AddChild(_options.line_space.keep_line_after_local_or_assign_statement.Element);
			}

			if (nextChild->GetType() == LuaAstNodeType::Comment)
			{
				auto comment = DiagnoseNode(nextChild);
				auto commentStatement = std::make_shared<StatementElement>();
				commentStatement->AddChild(comment);
				env->AddChild(commentStatement);
			}
			else
			{
				env->AddChild(DiagnoseNode(nextChild));
			}
		}

		++it;

		nextChild = ast_util::NextNode(it, children);
		if (nextChild == nullptr)
		{
			break;
		}
	}
	if (env->GetChildren().size() == 1)
	{
		return env->GetChildren()[0];
	}

	return env;
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseNodeAndBlockOrEnd(LuaAstNode::ChildIterator& it,
                                                                    const LuaAstNode::ChildrenContainer& children)
{
	auto env = std::make_shared<ExpressionElement>();
	auto node = *it;
	env->AddChild(DiagnoseNode(node));

	if (ast_util::NextMatch(it, LuaAstNodeType::Comment, children))
	{
		auto comment = ast_util::NextNode(it, children);
		int currentLine = _parser->GetLine(node->GetTextRange().EndOffset);
		int nextLine = _parser->GetLine(comment->GetTextRange().StartOffset);

		if (nextLine == currentLine)
		{
			env->Add<TextElement>(comment);
			++it;
		}
	}

	auto block = CalculateBlockFromParent(it, children);

	if (!block->GetChildren().empty())
	{
		if (_parser->GetLine(node->GetTextRange().StartOffset) != _parser->GetLine(block->GetTextRange().EndOffset))
		{
			env->AddChild(block);
		}
		else
		{
			for (auto blockChild : block->GetChildren())
			{
				if (blockChild->HasValidTextRange())
				{
					auto shortExpression = std::make_shared<ExpressionElement>();
					shortExpression->AddChildren(blockChild->GetChildren());
					env->AddChild(shortExpression);
				}
			}
		}
	}

	if (ast_util::NextMatch(it, LuaAstNodeType::KeyWord, children))
	{
		auto next = ast_util::NextNode(it, children);
		if (next->GetText() == "end")
		{
			env->Add<TextElement>(next, _options.white_space.CalculateSpace(next));
			++it;
		}
	}

	return env;
}

std::shared_ptr<FormatElement> LuaLinter::CalculateBlockFromParent(LuaAstNode::ChildIterator& it,
                                                                   const LuaAstNode::ChildrenContainer& children)
{
	std::shared_ptr<LuaAstNode> block = nullptr;
	std::vector<std::shared_ptr<LuaAstNode>> comments;

	for (; it != children.end(); ++it)
	{
		if (ast_util::NextMatch(it, LuaAstNodeType::Comment, children))
		{
			comments.push_back(ast_util::NextNode(it, children));
		}
		else if (ast_util::NextMatch(it, LuaAstNodeType::Block, children))
		{
			block = ast_util::NextNode(it, children);
		}
		else
		{
			break;
		}
	}

	if (block)
	{
		std::shared_ptr<LuaAstNode> copyBlock = block->Copy();

		for (auto comment : comments)
		{
			copyBlock->AddComment(comment);
		}

		return DiagnoseBlock(copyBlock);
	}
	else
	{
		auto indentElement = std::make_shared<IndentElement>();
		for (auto comment : comments)
		{
			auto commentStatement = std::make_shared<StatementElement>();
			commentStatement->Add<TextElement>(comment);
			indentElement->AddChild(commentStatement);
		}
		return indentElement;
	}
}

void LuaLinter::DefaultHandle(std::shared_ptr<LuaAstNode> node, std::shared_ptr<FormatElement> env)
{
	env->AddChild(DiagnoseNode(node));
}

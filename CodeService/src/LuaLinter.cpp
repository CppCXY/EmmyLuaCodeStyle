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
				auto childEnv = DiagnoseLabel(statement);
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
				// 基于这样的考虑 可能local 语句没有等号所以nameDefList的空格移上来
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
				env->AddChild(DiagnoseExpressionList(node));
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
					env->AddChild(DiagnopseNodeAndBlockOrEnd(it, children));
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
				else // 然而end是在 FormatNodeAndBlockOrEnd 中完成的
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
				env->AddChild(DiagnoseExpressionList(child));
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
				env->AddChild(FormatNameExpression(child));
				break;
			}
		case LuaAstNodeType::FunctionBody:
			{
				env->AddChild(FormatFunctionBody(child));
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

std::shared_ptr<FormatElement> LuaLinter::DiagnoseExpression(std::shared_ptr<LuaAstNode> expressionStatement,
                                                             std::shared_ptr<FormatElement> env)
{
}

std::shared_ptr<FormatElement> LuaLinter::DiagnoseExpressionList(std::shared_ptr<LuaAstNode> expressionList,
                                                                 std::shared_ptr<FormatElement> env)
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
		// 这个规则是下一个连续的赋值/local/注释语句如果和上一个赋值/local/注释语句 间距2行以上，则不认为是连续
		if (nextLine - currentLine > _options.align_style.MaxContinuousLineDistance)
		{
			break;
		}

		// 检查是否会是内联注释 比如 local t = 123 -- inline comment
		if ((currentChild->GetType() == LuaAstNodeType::LocalStatement
				|| currentChild->GetType() == LuaAstNodeType::AssignStatement)
			&& nextChild->GetType() == LuaAstNodeType::Comment && nextLine == currentLine)
		{
			auto lastStatementEnv = env->LastValidElement();
			if (lastStatementEnv)
			{
				lastStatementEnv->Add<TextElement>(nextChild);
			}
			//else 应该不存在这种情况
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
	// 如果不是和下文语句连续，则返回本身
	if (env->GetChildren().size() == 1)
	{
		return env->GetChildren()[0];
	}

	return env;
}

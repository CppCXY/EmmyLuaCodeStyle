#include "CodeService/LuaFormatter.h"
#include "CodeService/FormatElement/IndentElement.h"
#include "CodeService/FormatElement/StatementElement.h"
#include "CodeService/FormatElement/TextElement.h"
#include "CodeService/FormatElement/FormatContext.h"
#include "CodeService/FormatElement/KeepLineElement.h"
#include "CodeService/FormatElement/MinLineElement.h"
#include "CodeService/FormatElement/KeepBlankElement.h"
#include "CodeService/FormatElement/ExpressionElement.h"
#include "CodeService/FormatElement/LineElement.h"
#include "CodeService/FormatElement/AlignmentLayoutElement.h"
#include "CodeService/FormatElement/AlignToFirstElement.h"
#include "CodeService/FormatElement/KeepElement.h"
#include "CodeService/FormatElement/LongExpressionLayoutElement.h"
#include "CodeService/FormatElement/RangeFormatContext.h"
#include "CodeService/FormatElement/SubExpressionElement.h"

bool nextMatch(int currentIndex, LuaAstNodeType type, const std::vector<std::shared_ptr<LuaAstNode>>& vec)
{
	if (currentIndex >= 0 && (currentIndex + 1) < vec.size())
	{
		return vec[currentIndex + 1]->GetType() == type;
	}

	return false;
}

std::shared_ptr<LuaAstNode> nextNode(int currentIndex, const std::vector<std::shared_ptr<LuaAstNode>>& vec)
{
	if (currentIndex >= 0 && (currentIndex + 1) < vec.size())
	{
		return vec[currentIndex + 1];
	}

	return nullptr;
}


LuaFormatter::LuaFormatter(std::shared_ptr<LuaParser> luaParser, LuaCodeStyleOptions& options)
	: _parser(luaParser),
	  _options(options)
{
}

void LuaFormatter::BuildFormattedElement()
{
	auto chunkNode = _parser->GetAst();

	_env = FormatBlock(chunkNode->GetChildren()[0]);
}

std::string LuaFormatter::GetFormattedText()
{
	FormatContext ctx(_parser, _options);

	_env->Format(ctx);

	return ctx.GetText();
}

std::string LuaFormatter::GetRangeFormattedText(LuaFormatRange& validRange)
{
	RangeFormatContext ctx(_parser, _options, validRange);
	_env->Format(ctx);

	validRange = ctx.GetFormattedRange();

	return ctx.GetText();
}

std::vector<LuaDiagnosisInfo> LuaFormatter::GetDiagnosisInfos()
{
	DiagnosisContext ctx(_parser, _options);

	_env->DiagnosisCodeStyle(ctx);

	return ctx.GetDiagnosisInfos();
}

std::shared_ptr<FormatElement> LuaFormatter::FormatNode(std::shared_ptr<LuaAstNode> node)
{
	switch (node->GetType())
	{
	case LuaAstNodeType::Block:
		{
			return FormatBlock(node);
		}
	case LuaAstNodeType::LocalStatement:
		{
			return FormatLocalStatement(node);
		}
	case LuaAstNodeType::NameDefList:
		{
			return FormatNameDefList(node);
		}
	case LuaAstNodeType::ExpressionList:
		{
			return FormatExpressionList(node);
		}
	case LuaAstNodeType::Comment:
		{
			return FormatComment(node);
		}
	case LuaAstNodeType::LabelStatement:
		{
			return FormatLabelStatement(node);
		}
	case LuaAstNodeType::BreakStatement:
		{
			return FormatBreakStatement(node);
		}
	case LuaAstNodeType::GotoStatement:
		{
			return FormatGotoStatement(node);
		}
	case LuaAstNodeType::ReturnStatement:
		{
			return FormatReturnStatement(node);
		}
	case LuaAstNodeType::Identify:
		{
			return FormatIdentify(node);
		}
	case LuaAstNodeType::GeneralOperator:
		{
			return FormatGeneralOperator(node);
		}
	case LuaAstNodeType::KeyWord:
		{
			return FormatKeyWords(node);
		}
	case LuaAstNodeType::WhileStatement:
		{
			return FormatWhileStatement(node);
		}
	case LuaAstNodeType::ForStatement:
		{
			return FormatForStatement(node);
		}
	case LuaAstNodeType::ForBody:
		{
			return FormatForBody(node);
		}
	case LuaAstNodeType::Attribute:
		{
			return FormatAttribute(node);
		}
	case LuaAstNodeType::RepeatStatement:
		{
			return FormatRepeatStatement(node);
		}
	case LuaAstNodeType::DoStatement:
		{
			return FormatDoStatement(node);
		}
	case LuaAstNodeType::AssignStatement:
		{
			return FormatAssignment(node);
		}
	case LuaAstNodeType::IfStatement:
		{
			return FormatIfStatement(node);
		}
	case LuaAstNodeType::ExpressionStatement:
		{
			return FormatExpressionStatement(node);
		}
	case LuaAstNodeType::Expression:
		{
			return FormatExpression(node);
		}
	case LuaAstNodeType::CallExpression:
		{
			return FormatCallExpression(node);
		}
	case LuaAstNodeType::CallArgList:
		{
			return FormatCallArgList(node);
		}
	case LuaAstNodeType::FunctionStatement:
		{
			return FormatFunctionStatement(node);
		}
	case LuaAstNodeType::LocalFunctionStatement:
		{
			return FormatLocalFunctionStatement(node);
		}
	case LuaAstNodeType::ClosureExpression:
		{
			return FormatClosureExpression(node);
		}
	case LuaAstNodeType::NameExpression:
		{
			return FormatNameExpression(node);
		}
	case LuaAstNodeType::TableExpression:
		{
			return FormatTableExpression(node);
		}
	case LuaAstNodeType::TableField:
		{
			return FormatTableField(node);
		}
	case LuaAstNodeType::ParamList:
		{
			return FormatParamList(node);
		}
	case LuaAstNodeType::FunctionBody:
		{
			return FormatFunctionBody(node);
		}
	case LuaAstNodeType::LiteralExpression:
	default:
		{
			return std::make_shared<TextElement>(node->GetText(), node->GetTextRange());
		}
	}
}

std::shared_ptr<FormatElement> LuaFormatter::FormatBlock(std::shared_ptr<LuaAstNode> blockNode)
{
	auto indentEnv = std::make_shared<IndentElement>();

	auto statements = blockNode->GetChildren();

	for (int index = 0; index < statements.size(); index++)
	{
		auto statement = statements[index];
		switch (statement->GetType())
		{
		case LuaAstNodeType::AssignStatement:
		case LuaAstNodeType::LocalStatement:
			{
				if (nextMatch(index, LuaAstNodeType::AssignStatement, statements)
					|| nextMatch(index, LuaAstNodeType::LocalStatement, statements)
					|| nextMatch(index, LuaAstNodeType::Comment, statements))
				{
					indentEnv->AddChild(FormatAlignStatement(index, statements));
				}
				else
				{
					auto childEnv = FormatNode(statement);
					indentEnv->AddChild(childEnv);
				}
				indentEnv->AddChild(_options.keep_line_after_local_or_assign_statement);
				break;
			}
		case LuaAstNodeType::RepeatStatement:
			{
				indentEnv->AddChild(FormatNode(statement));
				indentEnv->AddChild(_options.keep_line_after_repeat_statement);
				break;
			}
		case LuaAstNodeType::DoStatement:
			{
				indentEnv->AddChild(FormatNode(statement));
				indentEnv->AddChild(_options.keep_line_after_do_statement);
				break;
			}
		case LuaAstNodeType::ForStatement:
			{
				indentEnv->AddChild(FormatNode(statement));
				indentEnv->AddChild(_options.keep_line_after_for_statement);
				break;
			}
		case LuaAstNodeType::WhileStatement:
			{
				indentEnv->AddChild(FormatNode(statement));
				indentEnv->AddChild(_options.keep_line_after_while_statement);
				break;
			}
		case LuaAstNodeType::IfStatement:
			{
				indentEnv->AddChild(FormatNode(statement));
				indentEnv->AddChild(_options.keep_line_after_if_statement);
				break;
			}
		case LuaAstNodeType::Comment:
			{
				auto comment = FormatNode(statement);
				auto commentStatement = std::make_shared<StatementElement>();
				commentStatement->AddChild(comment);
				indentEnv->AddChild(commentStatement);

				indentEnv->Add<KeepLineElement>();
				break;
			}
		case LuaAstNodeType::BreakStatement:
		case LuaAstNodeType::ReturnStatement:
		case LuaAstNodeType::GotoStatement:
		case LuaAstNodeType::ExpressionStatement:
			{
				auto statEnv = FormatNode(statement);
				if (nextMatch(index, LuaAstNodeType::Comment, statements))
				{
					auto next = statements[index + 1];
					int currentLine = _parser->GetLine(statement->GetTextRange().EndOffset);
					int nextLine = _parser->GetLine(next->GetTextRange().StartOffset);

					if (currentLine == nextLine)
					{
						statEnv->Add<KeepBlankElement>(1);
						statEnv->Add<TextElement>(next);
						index++;
					}
				}
				indentEnv->AddChild(statEnv);
				indentEnv->Add<KeepLineElement>();
				break;
			}
		case LuaAstNodeType::LocalFunctionStatement:
		case LuaAstNodeType::FunctionStatement:
			{
				indentEnv->AddChild(FormatNode(statement));
				indentEnv->AddChild(_options.keep_line_after_function_define_statement);
				break;
			}
		case LuaAstNodeType::LabelStatement:
			{
				auto childEnv = FormatNode(statement);
				if (_options.label_no_indent)
				{
					auto noIndent = std::make_shared<IndentElement>(0);
					noIndent->AddChild(childEnv);
					indentEnv->AddChild(noIndent);
				}
				else
				{
					indentEnv->AddChild(childEnv);
				}

				indentEnv->Add<KeepElement>(1);

				break;
			}
		default:
			{
				auto childEnv = FormatNode(statement);
				indentEnv->AddChild(childEnv);
				indentEnv->Add<KeepLineElement>();
			}
		}
	}

	return indentEnv;
}


std::shared_ptr<FormatElement> LuaFormatter::FormatLocalStatement(std::shared_ptr<LuaAstNode> localStatement)
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
				if (node->GetText() == "=")
				{
					env->Add<KeepBlankElement>(1);
				}

				env->Add<TextElement>(node);
				env->Add<KeepBlankElement>(1);
				break;
			}
		case LuaAstNodeType::NameDefList:
			{
				env->AddChild(FormatNode(node));
				break;
			}
		case LuaAstNodeType::ExpressionList:
			{
				env->AddChild(FormatNode(node));
				break;
			}
		default:
			DefaultHandle(node, env);
			break;
		}
	}

	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatAssignment(std::shared_ptr<LuaAstNode> assignStatement)
{
	auto env = std::make_shared<StatementElement>();
	bool isLeftExprList = true;
	for (auto& node : assignStatement->GetChildren())
	{
		switch (node->GetType())
		{
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<TextElement>(node);
				env->Add<KeepBlankElement>(1);
				break;
			}
		case LuaAstNodeType::ExpressionList:
			{
				env->AddChild(FormatNode(node));
				if (isLeftExprList)
				{
					env->Add<KeepBlankElement>(1);
					isLeftExprList = false;
				}

				break;
			}
		default:
			DefaultHandle(node, env);
			break;
		}
	}

	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatNameDefList(std::shared_ptr<LuaAstNode> nameDefList)
{
	auto env = std::make_shared<ExpressionElement>();

	for (auto node : nameDefList->GetChildren())
	{
		switch (node->GetType())
		{
		case LuaAstNodeType::Identify:
			{
				env->Add<TextElement>(node);
				break;
			}
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<TextElement>(node);
				env->Add<KeepBlankElement>(1);
				break;
			}
		default:
			DefaultHandle(node, env);
			break;
		}
	}

	return env;
}

/*
 * 表达式列表的格式化应该要求在换行后保持一致的缩进
 * 例如 local aaa = bbb,
 *				cccc,eeee
 *				     ,fff
 * 应当格式化为:
 * local aaa = bbb,
 *     cccc,eeee
 *	   ,fff
 */
std::shared_ptr<FormatElement> LuaFormatter::FormatExpressionList(std::shared_ptr<LuaAstNode> expressionList)
{
	auto env = std::make_shared<LongExpressionLayoutElement>(_options.continuation_indent);

	for (auto node : expressionList->GetChildren())
	{
		switch (node->GetType())
		{
		case LuaAstNodeType::Expression:
			{
				auto subEnv = std::make_shared<SubExpressionElement>();
				env->AddChild(FormatExpression(node, subEnv));
				env->Add<KeepElement>(0);
				break;
			}
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<TextElement>(node);
				env->Add<KeepElement>(1);
				break;
			}
		default:
			DefaultHandle(node, env);
			env->Add<KeepElement>(1);
		}
	}

	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatAssignLeftExpressionList(std::shared_ptr<LuaAstNode> expressionList)
{
	auto env = std::make_shared<LongExpressionLayoutElement>(_options.continuation_indent, true);

	for (auto node : expressionList->GetChildren())
	{
		switch (node->GetType())
		{
		case LuaAstNodeType::Expression:
			{
				auto subEnv = std::make_shared<SubExpressionElement>();
				env->AddChild(FormatExpression(node, subEnv));
				env->Add<KeepElement>(0);
				break;
			}
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<TextElement>(node);
				env->Add<KeepElement>(1);
				break;
			}
		default:
			DefaultHandle(node, env);
			env->Add<KeepElement>(1);
		}
	}

	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatComment(std::shared_ptr<LuaAstNode> comment)
{
	auto env = std::make_shared<ExpressionElement>();
	env->Add<TextElement>(comment);

	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatBreakStatement(std::shared_ptr<LuaAstNode> breakNode)
{
	auto env = std::make_shared<StatementElement>();

	for (auto child : breakNode->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
		case LuaAstNodeType::GeneralOperator:
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

std::shared_ptr<FormatElement> LuaFormatter::FormatReturnStatement(std::shared_ptr<LuaAstNode> returnNode)
{
	auto env = std::make_shared<StatementElement>();

	for (auto child : returnNode->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<TextElement>(child);
				break;
			}
		case LuaAstNodeType::ExpressionList:
			{
				env->Add<KeepBlankElement>(1);
				env->AddChild(FormatNode(child));
				break;
			}

		default:
			DefaultHandle(child, env);
			break;
		}
	}
	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatGotoStatement(std::shared_ptr<LuaAstNode> gotoNode)
{
	auto env = std::make_shared<StatementElement>();

	for (auto child : gotoNode->GetChildren())
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
				env->Add<KeepBlankElement>(1);
				env->AddChild(FormatNode(child));
				break;
			}

		default:
			DefaultHandle(child, env);
			break;
		}
	}
	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatLabelStatement(std::shared_ptr<LuaAstNode> labelStatement)
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
				env->AddChild(FormatNode(child));
				break;
			}

		default:
			DefaultHandle(child, env);
			break;
		}
	}
	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatIdentify(std::shared_ptr<LuaAstNode> identify)
{
	return std::make_shared<TextElement>(identify);
}

std::shared_ptr<FormatElement> LuaFormatter::FormatGeneralOperator(std::shared_ptr<LuaAstNode> general)
{
	return std::make_shared<TextElement>(general);
}

std::shared_ptr<FormatElement> LuaFormatter::FormatKeyWords(std::shared_ptr<LuaAstNode> keywords)
{
	return std::make_shared<TextElement>(keywords);
}

std::shared_ptr<FormatElement> LuaFormatter::FormatDoStatement(std::shared_ptr<LuaAstNode> doStatement)
{
	auto env = std::make_shared<StatementElement>();
	auto& children = doStatement->GetChildren();
	int i = 0;
	env->AddChild(FormatNodeAndBlockOrEnd(i, children));
	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatWhileStatement(std::shared_ptr<LuaAstNode> whileStatement)
{
	auto env = std::make_shared<StatementElement>();
	auto& children = whileStatement->GetChildren();
	for (int i = 0; i < children.size(); i++)
	{
		auto child = children[i];
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				if (child->GetText() == "do")
				{
					env->AddChild(FormatNodeAndBlockOrEnd(i, children));
					env->Add<KeepLineElement>();
				}
				else if (child->GetText() == "while")
				{
					env->Add<TextElement>(child);
					env->Add<KeepBlankElement>(1);
				}
				else
				{
					env->Add<TextElement>(child);
				}
				break;
			}
		case LuaAstNodeType::Expression:
			{
				auto expression = FormatNode(child);
				env->AddChild(expression);
				env->Add<KeepBlankElement>(1);
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

std::shared_ptr<FormatElement> LuaFormatter::FormatForStatement(std::shared_ptr<LuaAstNode> forStatement)
{
	auto env = std::make_shared<StatementElement>();
	for (auto child : forStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->Add<TextElement>(child);
				env->Add<KeepBlankElement>(1);
				break;
			}
		case LuaAstNodeType::ForNumber:
			{
				for (auto forNumberChild : child->GetChildren())
				{
					switch (forNumberChild->GetType())
					{
					case LuaAstNodeType::Identify:
					case LuaAstNodeType::GeneralOperator:
						{
							env->Add<TextElement>(forNumberChild);
							env->Add<KeepBlankElement>(1);
							break;
						}
					case LuaAstNodeType::Expression:
						{
							env->AddChild(FormatNode(forNumberChild));
							break;
						}
					case LuaAstNodeType::ForBody:
						{
							env->Add<KeepBlankElement>(1);
							env->AddChild(FormatNode(forNumberChild));
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
				for (auto forListChild : child->GetChildren())
				{
					switch (forListChild->GetType())
					{
					case LuaAstNodeType::KeyWord:
					case LuaAstNodeType::NameDefList:
					case LuaAstNodeType::ExpressionList:
						{
							env->AddChild(FormatNode(forListChild));
							env->Add<KeepBlankElement>(1);
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

std::shared_ptr<FormatElement> LuaFormatter::FormatForBody(std::shared_ptr<LuaAstNode> forBody)
{
	auto env = std::make_shared<ExpressionElement>();
	auto& children = forBody->GetChildren();
	for (int i = 0; i < children.size(); i++)
	{
		auto child = children[i];
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				if (child->GetText() == "do")
				{
					env->AddChild(FormatNodeAndBlockOrEnd(i, children));
					env->Add<KeepLineElement>();
				}
				else
				{
					env->Add<TextElement>(child);
				}
				break;
			}
		case LuaAstNodeType::Block:
			{
				env->AddChild(FormatNode(child));
				env->Add<KeepLineElement>();
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

std::shared_ptr<FormatElement> LuaFormatter::FormatAttribute(std::shared_ptr<LuaAstNode> attribute)
{
	auto env = std::make_shared<ExpressionElement>();

	for (auto child : attribute->GetChildren())
	{
		DefaultHandle(child, env);
	}

	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatRepeatStatement(std::shared_ptr<LuaAstNode> repeatStatement)
{
	auto env = std::make_shared<StatementElement>();
	auto& children = repeatStatement->GetChildren();
	for (int i = 0; i < children.size(); i++)
	{
		auto child = children[i];
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				if (child->GetText() == "repeat")
				{
					env->AddChild(FormatNodeAndBlockOrEnd(i, children));
					env->Add<KeepLineElement>();
				}
				else
				{
					env->Add<TextElement>(child);
					env->Add<KeepBlankElement>(1);
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

std::shared_ptr<FormatElement> LuaFormatter::FormatIfStatement(std::shared_ptr<LuaAstNode> ifStatement)
{
	auto env = std::make_shared<StatementElement>();
	auto& children = ifStatement->GetChildren();
	for (int i = 0; i < children.size(); i++)
	{
		auto child = children[i];
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				if (child->GetText() == "then" || child->GetText() == "else")
				{
					env->AddChild(FormatNodeAndBlockOrEnd(i, children));
					env->Add<KeepLineElement>();
				}
				else if (child->GetText() == "if" || child->GetText() == "elseif")
				{
					env->Add<TextElement>(child);
					env->Add<KeepBlankElement>(1);
				}
				else // 然而end是在 formatkeyAndBlockAndEnd 中完成的
				{
					env->Add<TextElement>(child);
				}

				break;
			}
		case LuaAstNodeType::Expression:
			{
				env->AddChild(FormatNode(child));
				env->Add<KeepBlankElement>(1);
				break;
			}
			// case LuaAstNodeType::Block:
			// 	{
			// 		env->AddChild(FormatNode(child));
			// 		env->Add<KeepLineElement>();
			// 		break;
			// 	}
		default:
			{
				DefaultHandle(child, env);
				break;
			}
		}
	}

	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatExpressionStatement(std::shared_ptr<LuaAstNode> expressionStatement)
{
	auto env = std::make_shared<StatementElement>();

	for (auto child : expressionStatement->GetChildren())
	{
		switch (child->GetType())
		{
			// 目前表达式语句上只有调用表达式
		case LuaAstNodeType::CallExpression:
			{
				env->AddChild(FormatNode(child));
				break;
			}
			// default 一般只有一个分号
		default:
			{
				DefaultHandle(child, env);
			}
		}
	}

	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatCallArgList(std::shared_ptr<LuaAstNode> callArgList)
{
	auto env = std::make_shared<ExpressionElement>();
	auto& children = callArgList->GetChildren();
	for (auto child : children)
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->AddChild(FormatNode(child));
				env->Add<KeepElement>(0);
				break;
			}
		case LuaAstNodeType::ExpressionList:
			{
				auto exprListEnv = FormatNode(child);
				if (_options.align_call_args)
				{
					auto alignToFirstEnv = std::make_shared<AlignToFirstElement>(_options.continuation_indent);
					alignToFirstEnv->AddChildren(exprListEnv->GetChildren());
					env->AddChild(alignToFirstEnv);
				}
				else
				{
					auto& exprListChildren = exprListEnv->GetChildren();
					auto keepElement = std::make_shared<KeepElement>(0);
					exprListChildren.insert(exprListChildren.begin(), keepElement);

					env->AddChild(exprListEnv);
				}

				if (_options.keep_one_space_between_call_args_and_bracket)
				{
					env->Add<KeepElement>(1);
				}
				else
				{
					env->Add<KeepElement>(0);
				}

				break;
			}
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<TextElement>(child);
				if (child->GetText() == ",")
				{
					env->Add<KeepElement>(1);
				}
				else if (child->GetText() == "(" && _options.keep_one_space_between_call_args_and_bracket && children.
					size() > 2)
				{
					env->Add<KeepElement>(1);
				}
				else
				{
					env->Add<KeepElement>(0);
				}
				break;
			}
		default:
			{
				DefaultHandle(child, env);
				env->Add<KeepElement>(1);
			}
		}
	}
	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatFunctionStatement(std::shared_ptr<LuaAstNode> functionStatement)
{
	auto env = std::make_shared<StatementElement>();

	for (auto child : functionStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->Add<TextElement>(child);
				env->Add<KeepBlankElement>(1);
				break;
			}
		case LuaAstNodeType::NameExpression:
			{
				env->AddChild(FormatNode(child));
				break;
			}
		case LuaAstNodeType::FunctionBody:
			{
				env->AddChild(FormatNode(child));
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

std::shared_ptr<FormatElement> LuaFormatter::FormatNameExpression(std::shared_ptr<LuaAstNode> nameExpression)
{
	auto env = std::make_shared<ExpressionElement>();

	for (auto child : nameExpression->GetChildren())
	{
		DefaultHandle(child, env);
	}
	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatParamList(std::shared_ptr<LuaAstNode> paramList)
{
	auto env = std::make_shared<ExpressionElement>();

	std::shared_ptr<FormatElement> paramListLayoutEnv = nullptr;

	if (_options.align_function_define_params)
	{
		paramListLayoutEnv = std::make_shared<AlignToFirstElement>(_options.continuation_indent);
	}
	else
	{
		paramListLayoutEnv = std::make_shared<LongExpressionLayoutElement>(_options.continuation_indent);
	}

	auto& children = paramList->GetChildren();
	for (int i = 0; i < static_cast<int>(children.size()); i++)
	{
		auto child = children[i];
		switch (child->GetType())
		{
		case LuaAstNodeType::GeneralOperator:
			{
				if (child->GetText() == ",")
				{
					paramListLayoutEnv->Add<TextElement>(child);
					paramListLayoutEnv->Add<KeepElement>(1);
				}
				else if (child->GetText() == ")")
				{
					paramListLayoutEnv->Add<TextElement>(child);
					env->AddChild(paramListLayoutEnv);
				}
				else
				{
					env->Add<TextElement>(child);
					env->Add<KeepElement>(0);
				}
				break;
			}
		case LuaAstNodeType::Param:
			{
				paramListLayoutEnv->Add<TextElement>(child);
				if (nextMatch(i, LuaAstNodeType::Comment, children))
				{
					paramListLayoutEnv->Add<KeepElement>(1);
				}
				else
				{
					paramListLayoutEnv->Add<KeepElement>(0);
				}

				break;
			}
		case LuaAstNodeType::Comment:
			{
				paramListLayoutEnv->Add<TextElement>(child);
				paramListLayoutEnv->Add<KeepElement>(1);
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

std::shared_ptr<FormatElement> LuaFormatter::FormatFunctionBody(std::shared_ptr<LuaAstNode> functionBody)
{
	int index = 0;
	return FormatNodeAndBlockOrEnd(index, functionBody->GetChildren());
}


std::shared_ptr<FormatElement> LuaFormatter::FormatClosureExpression(std::shared_ptr<LuaAstNode> closureExpression)
{
	auto env = std::make_shared<ExpressionElement>();

	for (auto child : closureExpression->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->Add<TextElement>(child);
				break;
			}
		case LuaAstNodeType::FunctionBody:
			{
				env->AddChild(FormatNode(child));
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

std::shared_ptr<FormatElement> LuaFormatter::FormatLocalFunctionStatement(
	std::shared_ptr<LuaAstNode> localFunctionStatement)
{
	auto env = std::make_shared<StatementElement>();

	for (auto child : localFunctionStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->Add<TextElement>(child);
				env->Add<KeepBlankElement>(1);
				break;
			}
		case LuaAstNodeType::Identify:
			{
				env->AddChild(FormatNode(child));
				break;
			}
		case LuaAstNodeType::FunctionBody:
			{
				env->AddChild(FormatNode(child));
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

/*
 * 表表达式有几种常见的排版行为
 * print {
 *	aaa,bbb,ddd,eee
 *  ffff = 123,
 *	aaaaa = 456,
 *	['agfwg'] = 123
 * }
 * 
 * local t = { a,b,c,d
 *			   e,f,g,i } 
 *
 * call { 
 * aa   = 1,
 * bbbb = 2,
 * dddd = 3,
 * ff   = 4,
 * ee   = 5,
 * ffff = 7
 * }
 *
 *
 */
std::shared_ptr<FormatElement> LuaFormatter::FormatTableExpression(std::shared_ptr<LuaAstNode> tableExpression)
{
	auto env = std::make_shared<ExpressionElement>();
	auto& children = tableExpression->GetChildren();
	for (int index = 0; index != children.size(); index++)
	{
		auto child = children[index];
		switch (child->GetType())
		{
		case LuaAstNodeType::GeneralOperator:
			{
				if (child->GetText() == "{")
				{
					env->Add<TextElement>(child);
					auto next = nextNode(index, children);
					if (next)
					{
						if (next->GetType() == LuaAstNodeType::GeneralOperator)
						{
							env->Add<KeepElement>(0);
							break;
						}

						env->Add<KeepElement>(_options.keep_one_space_between_table_and_bracket ? 1 : 0);
						index++;
						env->AddChild(FormatAlignTableField(index, children));
						env->Add<KeepElement>(_options.keep_one_space_between_table_and_bracket ? 1 : 0);
					}
				}
				else if (child->GetText() == "}")
				{
					env->Add<TextElement>(child);
				}

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

std::shared_ptr<FormatElement> LuaFormatter::FormatTableField(std::shared_ptr<LuaAstNode> tableField)
{
	auto env = std::make_shared<ExpressionElement>();

	for (auto child : tableField->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::GeneralOperator:
			{
				if (child->GetText() == "=")
				{
					env->Add<KeepBlankElement>(1);
					env->Add<TextElement>(child);
					env->Add<KeepBlankElement>(1);
				}
				else
				{
					env->Add<TextElement>(child);
				}
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

void LuaFormatter::DefaultHandle(std::shared_ptr<LuaAstNode> node, std::shared_ptr<FormatElement> envElement)
{
	auto childEnv = FormatNode(node);
	envElement->AddChild(childEnv);
}

std::shared_ptr<FormatElement> LuaFormatter::FormatAlignStatement(int& currentIndex,
                                                                  const std::vector<std::shared_ptr<LuaAstNode>>& vec)
{
	std::shared_ptr<FormatElement> env = nullptr;
	if (_options.continuous_assign_statement_align_to_equal_sign)
	{
		env = std::make_shared<AlignmentLayoutElement>();
	}
	else
	{
		env = std::make_shared<ExpressionElement>();
	}

	env->AddChild(FormatNode(vec[currentIndex]));

	if (currentIndex + 1 >= vec.size())
	{
		return env->GetChildren()[0];
	}
	auto nextChild = vec[currentIndex + 1];

	while (nextChild->GetType() == LuaAstNodeType::AssignStatement
		|| nextChild->GetType() == LuaAstNodeType::LocalStatement
		|| nextChild->GetType() == LuaAstNodeType::Comment)
	{
		auto currentChild = vec[currentIndex];
		int currentLine = _parser->GetLine(currentChild->GetTextRange().EndOffset);
		int nextLine = _parser->GetLine(nextChild->GetTextRange().StartOffset);
		// 这个规则是下一个连续的赋值/local/注释语句如果和上一个赋值/local/注释语句 间距2行以上，则不认为是连续
		if (nextLine - currentLine > 2)
		{
			break;
		}

		// 检查是否会是内联注释 比如 local t = 123 -- inline comment
		if ((currentChild->GetType() == LuaAstNodeType::LocalStatement || currentChild->GetType() ==
				LuaAstNodeType::AssignStatement) && nextChild->GetType() == LuaAstNodeType::Comment
			&& nextLine == currentLine)
		{
			auto lastStatementEnv = env->LastValidElement();
			if (lastStatementEnv)
			{
				lastStatementEnv->Add<KeepBlankElement>(1);
				lastStatementEnv->Add<TextElement>(nextChild);
			}
			//else 应该不存在这种情况
		}
		else
		{
			if ((currentChild->GetType() == LuaAstNodeType::LocalStatement || currentChild->GetType() ==
				LuaAstNodeType::AssignStatement))
			{
				env->AddChild(_options.keep_line_after_local_or_assign_statement);
			}
			else
			{
				env->Add<KeepLineElement>();
			}

			if (nextChild->GetType() == LuaAstNodeType::Comment)
			{
				auto comment = FormatNode(nextChild);
				auto commentStatement = std::make_shared<StatementElement>();
				commentStatement->AddChild(comment);
				env->AddChild(commentStatement);
			}
			else
			{
				env->AddChild(FormatNode(nextChild));
			}
		}

		currentIndex++;

		if (static_cast<std::size_t>(currentIndex) + 1 >= vec.size())
		{
			break;
		}

		nextChild = vec[static_cast<std::size_t>(currentIndex) + 1];
	}
	// 如果不是和下文语句连续，则返回本身
	if (env->GetChildren().size() == 1)
	{
		return env->GetChildren()[0];
	}

	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatAlignTableField(int& currentIndex,
                                                                   const std::vector<std::shared_ptr<LuaAstNode>>& vec)
{
	std::shared_ptr<FormatElement> env = nullptr;
	if (_options.align_table_field_to_first_field)
	{
		env = std::make_shared<AlignToFirstElement>(_options.indent);
	}
	else
	{
		env = std::make_shared<IndentElement>();
	}

	env->AddChild(FormatNode(vec[currentIndex]));

	if (currentIndex + 1 >= static_cast<int>(vec.size()))
	{
		return env;
	}

	auto nextChild = vec[currentIndex + 1];
	bool alignToEq = true;
	while (nextChild->GetType() == LuaAstNodeType::TableField
		|| nextChild->GetType() == LuaAstNodeType::TableFieldSep
		|| nextChild->GetType() == LuaAstNodeType::Comment)
	{
		auto currentChild = vec[currentIndex];
		int currentLine = _parser->GetLine(currentChild->GetTextRange().EndOffset);
		int nextLine = _parser->GetLine(nextChild->GetTextRange().StartOffset);
		if (nextLine == currentLine)
		{
			// 检查是否会是内联注释
			// 比如 t = 123, -- inline comment
			// 或者 c = 456 --fff
			// 或者 ddd = 123 --[[ffff]] ,
			if ((currentChild->GetType() == LuaAstNodeType::TableField || currentChild->GetType() ==
				LuaAstNodeType::TableFieldSep) && nextChild->GetType() == LuaAstNodeType::Comment)
			{
				env->Add<KeepBlankElement>(1);
				env->Add<TextElement>(nextChild);
			}
			else if ((currentChild->GetType() == LuaAstNodeType::TableFieldSep) && nextChild->GetType() ==
				LuaAstNodeType::TableField)
			{
				// 此时认为table 不应该考虑对齐到等号
				alignToEq = false;
				env->Add<KeepBlankElement>(1);
				env->AddChild(FormatNode(nextChild));
			}
			else if ((currentChild->GetType() == LuaAstNodeType::TableField) && nextChild->GetType() ==
				LuaAstNodeType::TableFieldSep)
			{
				env->AddChild(FormatNode(nextChild));
			}
			else
			{
				env->Add<KeepBlankElement>(1);
				env->AddChild(FormatNode(nextChild));
			}
		}
		else
		{
			env->Add<LineElement>();
			env->Add<KeepLineElement>();
			env->AddChild(FormatNode(nextChild));
		}

		currentIndex++;

		if ((currentIndex + 1) >= static_cast<int>(vec.size()))
		{
			break;
		}

		nextChild = vec[currentIndex + 1];
	}

	// 认为tableField 可以(但不是必须这样做)按照等号对齐
	if (alignToEq && _options.continuous_assign_table_field_align_to_equal_sign)
	{
		auto newEnv = std::make_shared<AlignToFirstElement>(_options.indent);
		auto alignmentLayoutElement = std::make_shared<AlignmentLayoutElement>();
		for (auto child : env->GetChildren())
		{
			alignmentLayoutElement->AddChild(child);
		}

		newEnv->AddChild(alignmentLayoutElement);

		env = newEnv;
	}

	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatNodeAndBlockOrEnd(int& currentIndex,
                                                                     const std::vector<std::shared_ptr<LuaAstNode>>&
                                                                     vec)
{
	auto env = std::make_shared<ExpressionElement>();
	auto keyNode = vec[currentIndex];
	env->AddChild(FormatNode(keyNode));

	if (nextMatch(currentIndex, LuaAstNodeType::Comment, vec))
	{
		auto comment = nextNode(currentIndex, vec);
		int currentLine = _parser->GetLine(keyNode->GetTextRange().EndOffset);
		int nextLine = _parser->GetLine(comment->GetTextRange().StartOffset);

		// 认为是内联注释
		if (nextLine == currentLine)
		{
			env->Add<KeepBlankElement>(1);
			env->Add<TextElement>(comment);
			currentIndex++;
		}
	}

	bool blockExist = false;
	auto block = FormatBlockFromParent(currentIndex, vec);


	if (!block->GetChildren().empty())
	{
		blockExist = true;
		if (_parser->GetLine(keyNode->GetTextRange().StartOffset) != _parser->GetLine(block->GetTextRange().EndOffset))
		{
			env->Add<KeepElement>(1);

			env->AddChild(block);
			env->Add<KeepElement>(1, true);
		}
		else
		{
			env->Add<KeepBlankElement>(1);

			for (auto blockChild : block->GetChildren())
			{
				if (blockChild->HasValidTextRange())
				{
					auto shortExpression = std::make_shared<ExpressionElement>();
					shortExpression->AddChildren(blockChild->GetChildren());
					env->AddChild(shortExpression);
					env->Add<KeepBlankElement>(1);
				}
			}
		}
	}
	else
	{
		env->Add<KeepElement>(1);
	}

	if (nextMatch(currentIndex, LuaAstNodeType::KeyWord, vec))
	{
		auto next = vec[currentIndex + 1];
		if (next->GetText() == "end" || next->GetText() == "else" || next->GetText() == "elseif")
		{
			env->Add<TextElement>(next);
			currentIndex++;
		}
		else if (!blockExist)
		{
			env->Add<LineElement>();
		}
	}
	else // 下一个不是关键词那能是什么那就换个行吧
	{
		env->Add<LineElement>();
	}

	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatBlockFromParent(int& currentIndex,
                                                                   const std::vector<std::shared_ptr<LuaAstNode>>& vec)
{
	std::shared_ptr<LuaAstNode> block = nullptr;
	std::vector<std::shared_ptr<LuaAstNode>> comments;

	for (; currentIndex < vec.size(); currentIndex++)
	{
		if (nextMatch(currentIndex, LuaAstNodeType::Comment, vec))
		{
			comments.push_back(nextNode(currentIndex, vec));
		}
		else if (nextMatch(currentIndex, LuaAstNodeType::Block, vec))
		{
			block = nextNode(currentIndex, vec);
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
			copyBlock->AddLeafChild(comment);
		}

		return FormatBlock(copyBlock);
	}
	else
	{
		auto indentElement = std::make_shared<IndentElement>();
		for (auto comment : comments)
		{
			auto commentStatement = std::make_shared<StatementElement>();
			commentStatement->Add<TextElement>(comment);
			indentElement->AddChild(commentStatement);
			indentElement->Add<KeepLineElement>();
		}

		return indentElement;
	}
}

void LuaFormatter::FormatSubExpressionNode(std::shared_ptr<LuaAstNode> expression,
                                           std::shared_ptr<FormatElement> env)
{
	switch (expression->GetType())
	{
	case LuaAstNodeType::BinaryExpression:
		{
			env->AddChild(FormatBinaryExpression(expression));
			break;
		}
	case LuaAstNodeType::UnaryExpression:
		{
			env->AddChild(FormatUnaryExpression(expression));
			break;
		}
	case LuaAstNodeType::IndexExpression:
		{
			env->AddChild(FormatIndexExpression(expression));
			break;
		}
	case LuaAstNodeType::PrimaryExpression:
		{
			env->AddChild(FormatPrimaryExpression(expression));
			break;
		}
	case LuaAstNodeType::CallExpression:
		{
			env->AddChild(FormatCallExpression(expression));
			break;
		}
	default:
		{
			DefaultHandle(expression, env);
		}
	}
}


/*
 * 表达式本身具有很大的复杂性
 * 如果表达式换行了，缩进多少又是很复杂的问题
 *
 */
std::shared_ptr<FormatElement> LuaFormatter::FormatExpression(std::shared_ptr<LuaAstNode> expression,
                                                              std::shared_ptr<FormatElement> env)
{
	if (env == nullptr)
	{
		env = std::make_shared<LongExpressionLayoutElement>(_options.continuation_indent);
	}

	auto& children = expression->GetChildren();
	for (int i = 0; i != children.size(); i++)
	{
		auto current = children[i];

		FormatSubExpressionNode(current, env);
		env->Add<KeepElement>(0);
	}

	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatBinaryExpression(std::shared_ptr<LuaAstNode> binaryExpression)
{
	auto env = std::make_shared<SubExpressionElement>();
	auto& children = binaryExpression->GetChildren();
	for (int i = 0; i < children.size(); i++)
	{
		auto current = children[i];

		FormatSubExpressionNode(current, env);
		env->Add<KeepElement>(1);
	}

	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatUnaryExpression(std::shared_ptr<LuaAstNode> unaryExpression)
{
	auto env = std::make_shared<SubExpressionElement>();

	for (auto child : unaryExpression->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::UnaryOperator:
			{
				env->Add<TextElement>(child);
				if (child->GetText() == "not")
				{
					env->Add<KeepElement>(1);
				}
				else
				{
					env->Add<KeepElement>(0);
				}
				break;
			}
		default:
			{
				FormatSubExpressionNode(child, env);
				env->Add<KeepElement>(1);
			}
		}
	}

	return env;
}


std::shared_ptr<FormatElement> LuaFormatter::FormatPrimaryExpression(std::shared_ptr<LuaAstNode> primaryExpression)
{
	auto env = std::make_shared<SubExpressionElement>();
	for (auto child : primaryExpression->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<TextElement>(child);
				env->Add<KeepElement>(0);
				break;
			}
		case LuaAstNodeType::Expression:
		case LuaAstNodeType::Identify:
			{
				env->AddChild(FormatNode(child));
				env->Add<KeepElement>(0);
				break;
			}
		default:
			{
				DefaultHandle(child, env);
				env->Add<KeepElement>(0);
			}
		}
	}
	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatIndexExpression(std::shared_ptr<LuaAstNode> indexExpression)
{
	auto env = std::make_shared<SubExpressionElement>();
	for (auto child : indexExpression->GetChildren())
	{
		FormatSubExpressionNode(child, env);
		if (child->GetType() == LuaAstNodeType::Comment)
		{
			env->Add<KeepElement>(1);
		}
		else
		{
			env->Add<KeepElement>(0);
		}
	}
	return env;
}


std::shared_ptr<FormatElement> LuaFormatter::FormatCallExpression(std::shared_ptr<LuaAstNode> callExpression)
{
	auto env = std::make_shared<SubExpressionElement>();
	auto& children = callExpression->GetChildren();
	for (int index = 0; index < children.size(); index++)
	{
		auto child = children[index];

		switch (child->GetType())
		{
		case LuaAstNodeType::IndexExpression:
		case LuaAstNodeType::PrimaryExpression:
			{
				FormatSubExpressionNode(child, env);

				auto next = nextNode(index, children);
				if (next && next->GetType() == LuaAstNodeType::CallArgList)
				{
					if (next->GetChildren().size() == 1)
					{
						env->Add<KeepElement>(1);
					}
					else
					{
						env->Add<KeepElement>(0);
					}
				}
				else
				{
					env->Add<KeepElement>(1);
				}

				break;
			}
		case LuaAstNodeType::CallArgList:
			{
				env->AddChild(FormatNode(child));
				break;
			}
		default:
			{
				DefaultHandle(child, env);
				env->Add<KeepElement>(1);
			}
		}
	}
	return env;
}

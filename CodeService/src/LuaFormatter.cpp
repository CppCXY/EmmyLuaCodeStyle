#include "CodeService/LuaFormatter.h"

#include <iostream>

#include "CodeService/FormatElement/IndentElement.h"
#include "CodeService/FormatElement/StatementElement.h"
#include "CodeService/FormatElement/TextElement.h"
#include "CodeService/FormatElement/ExpressionElement.h"
#include "CodeService/FormatElement/FormatContext.h"

bool nextMatch(int currentIndex, LuaAstNodeType type, std::vector<std::shared_ptr<LuaAstNode>>& vec)
{
	if (currentIndex >= 0 && (currentIndex + 1) < vec.size())
	{
		return vec[currentIndex + 1]->GetType() == type;
	}

	return false;
}

std::shared_ptr<LuaAstNode> nextNode(int currentIndex, std::vector<std::shared_ptr<LuaAstNode>>& vec)
{
	if (currentIndex >= 0 && (currentIndex + 1) < vec.size())
	{
		return vec[currentIndex + 1];
	}

	return nullptr;
}


LuaFormatter::LuaFormatter(std::shared_ptr<LuaParser> luaParser, LuaFormatOptions& options)
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
	LuaFormatOptions options;
	FormatContext ctx(_parser, options);

	_env->Serialize(ctx);

	return ctx.GetText();
}

void LuaFormatter::AddDiagnosis(TextRange range, std::string_view message)
{
}

std::vector<LuaCodeDiagnosis>& LuaFormatter::GetDiagnosis()
{
	return _diagnosis;
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

	for (std::size_t index = 0; index != statements.size(); index++)
	{
		auto statement = statements[index];
		switch (statement->GetType())
		{
		case LuaAstNodeType::AssignStatement:
		case LuaAstNodeType::LocalStatement:
			{
				// if (nextMatch(index, LuaAstNodeType::AssignStatement, statements)
				// 	|| nextMatch(index, LuaAstNodeType::LocalStatement, statements)
				// 	|| nextMatch(index, LuaAstNodeType::ShortComment, statements))
				// {
				// 	// AttemptAlignment()
				// }
				// else
				// {
				auto childEnv = FormatNode(statement);
				indentEnv->AddChild(childEnv);
				indentEnv->KeepLine();
				// }
				break;
			}
		case LuaAstNodeType::RepeatStatement:
		case LuaAstNodeType::DoStatement:
		case LuaAstNodeType::ForStatement:
		case LuaAstNodeType::WhileStatement:
			{
				auto childEnv = FormatNode(statement);
				indentEnv->AddChild(childEnv);

				indentEnv->MinLine(1);
				break;
			}
		case LuaAstNodeType::ShortComment:
		case LuaAstNodeType::LongComment:
		case LuaAstNodeType::ShebangComment:
		case LuaAstNodeType::BreakStatement:
		case LuaAstNodeType::ReturnStatement:
		case LuaAstNodeType::GotoStatement:
		case LuaAstNodeType::ExpressionStatement:
			{
				auto childEnv = FormatNode(statement);
				indentEnv->AddChild(childEnv);

				indentEnv->KeepLine();
				break;
			}
		case LuaAstNodeType::FunctionStatement:
			{
				auto childEnv = FormatNode(statement);
				indentEnv->AddChild(childEnv);

				if (nextMatch(index, LuaAstNodeType::FunctionStatement, statements))
				{
					indentEnv->KeepLine(1);
				}
				else
				{
					indentEnv->MinLine(1);
				}
				break;
			}
		default:
			{
				auto childEnv = FormatNode(statement);
				indentEnv->AddChild(childEnv);
				indentEnv->KeepLine();
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
				env->AddChild(node);
				env->KeepBlank(1);
				break;
			}
		case LuaAstNodeType::NameDefList:
			{
				env->AddChild(FormatNode(node));
				env->KeepBlank(1);
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

std::shared_ptr<FormatElement> LuaFormatter::FormatNameDefList(std::shared_ptr<LuaAstNode> nameDefList)
{
	auto env = std::make_shared<ExpressionElement>();

	for (auto node : nameDefList->GetChildren())
	{
		switch (node->GetType())
		{
		case LuaAstNodeType::Identify:
			{
				env->AddChild(node);
				break;
			}
		case LuaAstNodeType::GeneralOperator:
			{
				env->AddChild(node);
				env->KeepBlank(1);
				break;
			}
		default:
			DefaultHandle(node, env);
		}
	}

	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatExpressionList(std::shared_ptr<LuaAstNode> expressionList)
{
	auto env = std::make_shared<ExpressionElement>();

	for (auto node : expressionList->GetChildren())
	{
		switch (node->GetType())
		{
		case LuaAstNodeType::Expression:
			{
				env->AddChild(node);
				break;
			}
		case LuaAstNodeType::GeneralOperator:
			{
				env->AddChild(node);
				env->KeepBlank(1);
				break;
			}
		default:
			DefaultHandle(node, env);
		}
	}

	return env;
}

void LuaFormatter::DefaultHandle(std::shared_ptr<LuaAstNode> node, std::shared_ptr<FormatElement> envElement)
{
	auto childEnv = FormatNode(node);
	envElement->AddChild(childEnv);
}

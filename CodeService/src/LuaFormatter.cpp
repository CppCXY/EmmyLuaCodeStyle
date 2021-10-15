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
	case LuaAstNodeType::LongComment:
	case LuaAstNodeType::ShortComment:
	case LuaAstNodeType::ShebangComment:
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

std::shared_ptr<FormatElement> LuaFormatter::FormatAssignment(std::shared_ptr<LuaAstNode> assignStatement)
{
	auto env = std::make_shared<StatementElement>();
	for (auto& node : assignStatement->GetChildren())
	{
		switch (node->GetType())
		{
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
			break;
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

std::shared_ptr<FormatElement> LuaFormatter::FormatComment(std::shared_ptr<LuaAstNode> comment)
{
	auto env = std::make_shared<StatementElement>();
	env->AddChild(comment);
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
				env->AddChild(child);
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
				env->AddChild(child);
				break;
			}
		case LuaAstNodeType::ExpressionList:
			{
				env->KeepBlank(1);
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
				env->AddChild(child);
				break;
			}
		case LuaAstNodeType::Identify:
			{
				env->KeepBlank(1);
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
				env->AddChild(child);
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
	return std::make_shared<TextElement>(identify->GetText(), identify->GetTextRange());
}

std::shared_ptr<FormatElement> LuaFormatter::FormatGeneralOperator(std::shared_ptr<LuaAstNode> general)
{
	return std::make_shared<TextElement>(general->GetText(), general->GetTextRange());
}

std::shared_ptr<FormatElement> LuaFormatter::FormatKeyWords(std::shared_ptr<LuaAstNode> keywords)
{
	return std::make_shared<TextElement>(keywords->GetText(), keywords->GetTextRange());
}

std::shared_ptr<FormatElement> LuaFormatter::FormatDoStatement(std::shared_ptr<LuaAstNode> doStatement)
{
	auto env = std::make_shared<StatementElement>();

	for (auto child : doStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->AddChild(child);
				if (child->GetText() == "do")
				{
					env->AddLine();
				}
				break;
			}
		case LuaAstNodeType::Block:
			{
				env->AddChild(FormatBlock(child));
				env->AddLine();
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

std::shared_ptr<FormatElement> LuaFormatter::FormatWhileStatement(std::shared_ptr<LuaAstNode> whileStatement)
{
	auto env = std::make_shared<StatementElement>();
	for (auto child : whileStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->AddChild(child);
				if (child->GetText() == "do")
				{
					env->AddLine();
				}
				break;
			}
		case LuaAstNodeType::Expression:
			{
				auto expression = FormatNode(child);
				env->AddChild(expression);
				env->KeepBlank(1);
				break;
			}
		case LuaAstNodeType::Block:
			{
				env->AddChild(FormatNode(child));
				env->AddLine();
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
				env->AddChild(child);
				env->KeepBlank(1);
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
							env->AddChild(forNumberChild);
							env->KeepBlank(1);
							break;
						}
					case LuaAstNodeType::Expression:
						{
							env->AddChild(FormatNode(forNumberChild));
							break;
						}
					case LuaAstNodeType::ForBody:
						{
							env->KeepBlank(1);
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
							env->KeepBlank(1);
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
	for (auto child : forBody->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->AddChild(child);
				if (child->GetText() == "do")
				{
					env->AddLine();
				}
				break;
			}
		case LuaAstNodeType::Block:
			{
				env->AddChild(FormatNode(child));
				env->AddLine();
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

	for (auto child : repeatStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->AddChild(child);
				if (child->GetText() == "repeat")
				{
					env->AddLine();
				}
				break;
			}
		case LuaAstNodeType::Block:
			{
				env->AddChild(FormatNode(child));
				env->AddLine();
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

void LuaFormatter::DefaultHandle(std::shared_ptr<LuaAstNode> node, std::shared_ptr<FormatElement> envElement)
{
	auto childEnv = FormatNode(node);
	envElement->AddChild(childEnv);
}

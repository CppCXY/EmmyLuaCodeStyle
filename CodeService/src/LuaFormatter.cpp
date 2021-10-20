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

	_env->Serialize(ctx, 0, nullptr);

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
	case LuaAstNodeType::BinaryExpression:
		{
			return FormatBinaryExpression(node);
		}
	case LuaAstNodeType::UnaryExpression:
		{
			return FormatUnaryExpression(node);
		}
	case LuaAstNodeType::PrimaryExpression:
		{
			return FormatPrimaryExpression(node);
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
	case LuaAstNodeType::IndexExpression:
		{
			return FormatIndexExpression(node);
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
				indentEnv->Add<KeepLineElement>();
				break;
			}
		case LuaAstNodeType::RepeatStatement:
		case LuaAstNodeType::DoStatement:
		case LuaAstNodeType::ForStatement:
		case LuaAstNodeType::WhileStatement:
			{
				auto childEnv = FormatNode(statement);
				indentEnv->AddChild(childEnv);

				indentEnv->Add<MinLineElement>(1);
				break;
			}
		case LuaAstNodeType::Comment:
		case LuaAstNodeType::BreakStatement:
		case LuaAstNodeType::ReturnStatement:
		case LuaAstNodeType::GotoStatement:
		case LuaAstNodeType::ExpressionStatement:
			{
				indentEnv->AddChild(FormatNode(statement));

				indentEnv->Add<KeepLineElement>();
				break;
			}
		case LuaAstNodeType::LocalFunctionStatement:
		case LuaAstNodeType::FunctionStatement:
			{
				auto childEnv = FormatNode(statement);
				indentEnv->AddChild(childEnv);

				if (nextMatch(index, LuaAstNodeType::FunctionStatement, statements))
				{
					indentEnv->Add<KeepLineElement>(1);
				}
				else
				{
					indentEnv->Add<MinLineElement>(1);
				}
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
				env->Add<TextElement>(node);
				env->Add<KeepBlankElement>(1);
				break;
			}
		case LuaAstNodeType::NameDefList:
			{
				env->AddChild(FormatNode(node));
				env->Add<KeepBlankElement>(1);
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

std::shared_ptr<FormatElement> LuaFormatter::FormatExpressionList(std::shared_ptr<LuaAstNode> expressionList)
{
	auto env = std::make_shared<ExpressionElement>();

	for (auto node : expressionList->GetChildren())
	{
		switch (node->GetType())
		{
		case LuaAstNodeType::Expression:
			{
				env->AddChild(FormatNode(node));
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
		}
	}

	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatComment(std::shared_ptr<LuaAstNode> comment)
{
	auto env = std::make_shared<StatementElement>();
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

	for (auto child : doStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->Add<TextElement>(child);
				if (child->GetText() == "do")
				{
					env->Add<LineElement>();
				}
				break;
			}
		case LuaAstNodeType::Block:
			{
				env->AddChild(FormatBlock(child));
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

std::shared_ptr<FormatElement> LuaFormatter::FormatWhileStatement(std::shared_ptr<LuaAstNode> whileStatement)
{
	auto env = std::make_shared<StatementElement>();
	for (auto child : whileStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->Add<TextElement>(child);
				if (child->GetText() == "do")
				{
					env->Add<LineElement>();
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
	for (auto child : forBody->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->Add<TextElement>(child);
				if (child->GetText() == "do")
				{
					env->Add<LineElement>();
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

	for (auto child : repeatStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->Add<TextElement>(child);
				if (child->GetText() == "repeat")
				{
					env->Add<LineElement>();
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

std::shared_ptr<FormatElement> LuaFormatter::FormatIfStatement(std::shared_ptr<LuaAstNode> ifStatement)
{
	auto env = std::make_shared<StatementElement>();

	for (auto child : ifStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->Add<TextElement>(child);
				if (child->GetText() == "then" || child->GetText() == "else")
				{
					env->Add<LineElement>();
				}
				else if (child->GetText() == "if" || child->GetText() == "elseif")
				{
					env->Add<KeepBlankElement>(1);
				}
				break;
			}
		case LuaAstNodeType::Expression:
			{
				env->AddChild(FormatNode(child));
				env->Add<KeepBlankElement>(1);
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

std::shared_ptr<FormatElement> LuaFormatter::FormatExpression(std::shared_ptr<LuaAstNode> expression)
{
	auto env = std::make_shared<ExpressionElement>();

	for (auto child : expression->GetChildren())
	{
		// switch (child->GetType())
		// {
		// 	// case LuaAstNodeType::BinaryExpression:
		// 	// 	{
		// 	// 	}
		// default:
		// 	{
		DefaultHandle(child, env);
		// 		break;
		// 	}
		// }
	}

	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatBinaryExpression(std::shared_ptr<LuaAstNode> binaryExpression)
{
	auto env = std::make_shared<ExpressionElement>();

	for (auto child : binaryExpression->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::BinaryOperator:
			{
				env->Add<KeepBlankElement>(1);
				env->Add<TextElement>(child);
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

std::shared_ptr<FormatElement> LuaFormatter::FormatUnaryExpression(std::shared_ptr<LuaAstNode> unaryExpression)
{
	auto env = std::make_shared<ExpressionElement>();

	for (auto child : unaryExpression->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::UnaryOperator:
			{
				env->Add<TextElement>(child);
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

std::shared_ptr<FormatElement> LuaFormatter::FormatCallExpression(std::shared_ptr<LuaAstNode> callExpression)
{
	auto env = std::make_shared<ExpressionElement>();
	for (auto child : callExpression->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::PrimaryExpression:
			{
				env->AddChild(FormatNode(child));
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
			}
		}
	}
	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatCallArgList(std::shared_ptr<LuaAstNode> callArgList)
{
	auto env = std::make_shared<ExpressionElement>();

	for (auto child : callArgList->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<TextElement>(child);
				if (child->GetText() == ",")
				{
					env->Add<KeepBlankElement>(1);
				}
				break;
			}
		case LuaAstNodeType::Expression:
		case LuaAstNodeType::Identify:
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


std::shared_ptr<FormatElement> LuaFormatter::FormatPrimaryExpression(std::shared_ptr<LuaAstNode> primaryExpression)
{
	auto env = std::make_shared<ExpressionElement>();

	for (auto child : primaryExpression->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<TextElement>(child);
				break;
			}
		case LuaAstNodeType::Expression:
		case LuaAstNodeType::Identify:
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

	for (auto child : paramList->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<TextElement>(child);
				if (child->GetText() == ",")
				{
					env->Add<KeepBlankElement>(1);
				}
				break;
			}
		case LuaAstNodeType::Identify:
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

std::shared_ptr<FormatElement> LuaFormatter::FormatFunctionBody(std::shared_ptr<LuaAstNode> functionBody)
{
	auto env = std::make_shared<ExpressionElement>();

	for (auto child : functionBody->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::ParamList:
			{
				env->AddChild(FormatNode(child));
				env->Add<LineElement>();
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
			}
		}
	}
	return env;
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


std::shared_ptr<FormatElement> LuaFormatter::FormatTableExpression(std::shared_ptr<LuaAstNode> tableExpression)
{
	auto env = std::make_shared<ExpressionElement>();

	for (auto child : tableExpression->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::GeneralOperator:
			{
				if (tableExpression->GetChildren().size() > 2)
				{
					if (child->GetText() == "{")
					{
						env->Add<TextElement>(child);
						env->Add<KeepBlankElement>(1);
					}
					else if (child->GetText() == "}")
					{
						env->Add<KeepBlankElement>(1);
						env->Add<TextElement>(child);
					}
				}
				else
				{
					env->Add<TextElement>(child);
				}
				break;
			}
		case LuaAstNodeType::TableField:
			{
				env->AddChild(FormatNode(child));
				break;
			}
		case LuaAstNodeType::TableFieldSep:
			{
				env->AddChild(FormatNode(child));
				env->Add<KeepBlankElement>(1);
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

std::shared_ptr<FormatElement> LuaFormatter::FormatIndexExpression(std::shared_ptr<LuaAstNode> indexExpression)
{
	auto env = std::make_shared<ExpressionElement>();

	for (auto child : indexExpression->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::IndexOperator:
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


void LuaFormatter::DefaultHandle(std::shared_ptr<LuaAstNode> node, std::shared_ptr<FormatElement> envElement)
{
	auto childEnv = FormatNode(node);
	envElement->AddChild(childEnv);
}

std::shared_ptr<FormatElement> LuaFormatter::FormatAlignStatement(int& currentIndex,
                                                                  std::vector<std::shared_ptr<LuaAstNode>>& vec)
{
	auto env = std::make_shared<AlignmentLayoutElement>();

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
			auto& lastStatementEnv = env->GetChildren().back();

			lastStatementEnv->Add<KeepBlankElement>(1);
			lastStatementEnv->Add<TextElement>(nextChild);
		}
		else
		{
			env->AddChild(FormatNode(nextChild));
			env->Add<KeepLineElement>();
		}

		currentIndex++;

		if ((currentIndex + 1) >= vec.size())
		{
			break;
		}

		nextChild = vec[currentIndex + 1];
	}
	// 如果不是和下文语句连续，则返回本身
	if (env->GetChildren().size() == 1)
	{
		return env->GetChildren()[0];
	}

	return env;
}

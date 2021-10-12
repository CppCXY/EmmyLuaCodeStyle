#include "CodeService/LuaFormatter.h"
#include "CodeService/FormatElement/IndentElement.h"


bool nextMatch(int currentIndex, LuaAstNodeType type, std::vector<std::shared_ptr<LuaAstNode>> vec)
{
	if (currentIndex >= 0 && (currentIndex + 1) < vec.size())
	{
		return vec[currentIndex + 1]->GetType() == type;
	}

	return false;
}

std::shared_ptr<LuaAstNode> nextNode(int currentIndex, std::vector<std::shared_ptr<LuaAstNode>> vec)
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
}

std::string LuaFormatter::GetFormattedText()
{
	return {};
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
	default:
		{
			return std::make_shared<FormatElement>(node);
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
				if (nextMatch(index, LuaAstNodeType::AssignStatement, statements)
					|| nextMatch(index, LuaAstNodeType::LocalStatement, statements)
					|| nextMatch(index, LuaAstNodeType::ShortComment, statements))
				{
					// AttemptAlignment()
				}
				else
				{
					auto childEnv = FormatNode(statement);
					indentEnv->AddChild(childEnv);
				}
				break;
			}
		case LuaAstNodeType::ShortComment:
		case LuaAstNodeType::LongComment:
		case LuaAstNodeType::ShebangComment:
			{
				auto childEnv = FormatNode(statement);
				indentEnv->AddChild(childEnv);
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

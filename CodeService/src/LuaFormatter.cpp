#include "CodeService/LuaFormatter.h"
#include "CodeService/FormatElement/IndentElement.h"
#include "CodeService/FormatElement/StatementElement.h"
#include "CodeService/FormatElement/TextElement.h"
#include "CodeService/FormatElement/KeepLineElement.h"
#include "CodeService/FormatElement/SpaceElement.h"
#include "CodeService/FormatElement/ExpressionElement.h"
#include "CodeService/FormatElement/LineElement.h"
#include "CodeService/FormatElement/AlignmentLayoutElement.h"
#include "CodeService/FormatElement/AlignToFirstElement.h"
#include "CodeService/FormatElement/KeepElement.h"
#include "CodeService/FormatElement/LongExpressionLayoutElement.h"
#include "CodeService/FormatElement/RangeFormatContext.h"
#include "CodeService/FormatElement/SubExpressionElement.h"
#include "CodeService/NameStyle/NameStyleChecker.h"
#include "CodeService/FormatElement/NoIndentElement.h"
#include "CodeService/FormatElement/SerializeContext.h"
#include "CodeService/FormatElement/IndentOnLineBreakElement.h"
#include "CodeService/FormatElement/SepElement.h"
#include "CodeService/FormatElement/AlignIfLayoutElement.h"
#include "CodeService/FormatElement/MaxSpaceElement.h"
#include "CodeService/FormatElement/StringLiteralElement.h"
#include "CodeService/FormatElement/CallArgsListLayoutElement.h"
#include "Util/StringUtil.h"
#include "CodeService/AstUtil.h"
#include "CodeService/FormatElement/KeyWordElement.h"
#include "CodeService/FormatElement/OperatorElement.h"
#include "LuaParser/LuaTokenTypeDetail.h"

bool NextMatch(LuaAstNode::ChildIterator it, LuaAstNodeType type, const LuaAstNode::ChildrenContainer& container)
{
	if (it != container.end() && (++it) != container.end())
	{
		return (*it)->GetType() == type;
	}

	return false;
}

std::shared_ptr<LuaAstNode> NextNode(LuaAstNode::ChildIterator it, const LuaAstNode::ChildrenContainer& container)
{
	if (it != container.end() && (++it) != container.end())
	{
		return *it;
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
	if (!chunkNode->GetChildren().empty())
	{
		_env = FormatBlock(chunkNode->GetChildren().front());
	}
	else
	{
		_env = std::make_shared<IndentElement>();
	}
}

std::string LuaFormatter::GetFormattedText()
{
	SerializeContext ctx(_parser, _options);
	ctx.SetReadySize(_parser->GetSource().size() + _parser->GetSource().size() / 2);

	_env->Format(ctx);

	return ctx.GetText();
}

void LuaFormatter::BuildRangeFormattedElement(LuaFormatRange& validRange)
{
	// auto chunkNode = _parser->GetAst();
	// if (!chunkNode->GetChildren().empty())
	// {
	// 	auto fileBlock = chunkNode->GetChildren().front();
	// 	_env = FormatRangeBlock(fileBlock, validRange);
	// }
	// else
	// {
	// 	_env = std::make_shared<IndentElement>();
	// }
}

std::string LuaFormatter::GetRangeFormattedText(LuaFormatRange& validRange)
{
	RangeFormatContext ctx(_parser, _options, validRange);
	_env->Format(ctx);

	validRange = ctx.GetFormattedRange();

	return ctx.GetText();
}

void LuaFormatter::CalculateDiagnosisInfos(DiagnosisContext& ctx)
{
	_env->DiagnosisCodeStyle(ctx);
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
	case LuaAstNodeType::NameIdentify:
		{
			return FormatNameIdentify(node);
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
	case LuaAstNodeType::StringLiteralExpression:
		{
			return FormatStringLiteralExpression(node);
		}
	case LuaAstNodeType::TableFieldSep:
		{
			return FormatTableSep(node);
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

	auto& statements = blockNode->GetChildren();

	for (auto it = statements.begin(); it != statements.end(); ++it)
	{
		const auto statement = *it;
		switch (statement->GetType())
		{
		case LuaAstNodeType::AssignStatement:
		case LuaAstNodeType::LocalStatement:
			{
				if (!indentEnv->IsDisableEnv()
					&& (NextMatch(it, LuaAstNodeType::AssignStatement, statements)
						|| NextMatch(it, LuaAstNodeType::LocalStatement, statements)
						|| NextMatch(it, LuaAstNodeType::Comment, statements)))
				{
					indentEnv->AddChild(FormatAlignStatement(it, statements));
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
				auto child = FormatRepeatStatement(statement);
				indentEnv->AddChild(child);
				indentEnv->AddChild(_options.keep_line_after_repeat_statement);
				break;
			}
		case LuaAstNodeType::DoStatement:
			{
				indentEnv->AddChild(FormatDoStatement(statement));
				indentEnv->AddChild(_options.keep_line_after_do_statement);
				break;
			}
		case LuaAstNodeType::ForStatement:
			{
				indentEnv->AddChild(FormatForStatement(statement));
				indentEnv->AddChild(_options.keep_line_after_for_statement);
				break;
			}
		case LuaAstNodeType::WhileStatement:
			{
				indentEnv->AddChild(FormatWhileStatement(statement));
				indentEnv->AddChild(_options.keep_line_after_while_statement);
				break;
			}
		case LuaAstNodeType::IfStatement:
			{
				indentEnv->AddChild(FormatIfStatement(statement));
				indentEnv->AddChild(_options.keep_line_after_if_statement);
				break;
			}
		case LuaAstNodeType::ExpressionStatement:
			{
				indentEnv->AddChild(FormatExpressionStatement(statement));
				indentEnv->AddChild(_options.keep_line_after_expression_statement);
				break;
			}
		case LuaAstNodeType::LocalFunctionStatement:
		case LuaAstNodeType::FunctionStatement:
			{
				indentEnv->AddChild(FormatNode(statement));
				indentEnv->AddChild(_options.keep_line_after_function_define_statement);
				break;
			}
		case LuaAstNodeType::BreakStatement:
		case LuaAstNodeType::ReturnStatement:
		case LuaAstNodeType::GotoStatement:
			{
				indentEnv->AddChild(FormatNode(statement));
				indentEnv->Add<KeepLineElement>();
				break;
			}
		case LuaAstNodeType::Comment:
			{
				auto last = indentEnv->LastValidElement();
				if (last && _parser->GetLine(last->GetTextRange().EndOffset)
					== _parser->GetLine(statement->GetTextRange().StartOffset))
				{
					if (!last->GetChildren().empty() && last->GetChildren().back()->HasValidTextRange())
					{
						last->TrimEnd();
						last->Add<SpaceElement>(_options.statement_inline_comment_space);
					}
					last->AddChild(FormatComment(statement));
				}
				else
				{
					auto comment = FormatComment(statement);
					auto commentStatement = std::make_shared<StatementElement>();
					commentStatement->AddChild(comment);
					indentEnv->AddChild(commentStatement);
					indentEnv->Add<KeepLineElement>();

					// emmylua doc ---@format
					auto shortComment = statement->FindFirstOf(LuaAstNodeType::ShortComment);
					if (shortComment)
					{
						auto docTagFormat = shortComment->FindFirstOf(LuaAstNodeType::DocTagFormat);
						if (docTagFormat)
						{
							auto disableKeyWorld = docTagFormat->FindFirstOf(LuaAstNodeType::KeyWord);
							if (disableKeyWorld)
							{
								if (disableKeyWorld->GetTokenType() == TK_DOC_DISABLE)
								{
									indentEnv->EnableDisableFormat();
								}
								else if (disableKeyWorld->GetTokenType() == TK_DOC_DISABLE_NEXT)
								{
									indentEnv->EnableDisableNext();
								}
							}
						}
					}
				}
				break;
			}
		case LuaAstNodeType::LabelStatement:
			{
				auto childEnv = FormatLabelStatement(statement);
				if (_options.label_no_indent)
				{
					auto noIndent = std::make_shared<NoIndentElement>();
					noIndent->AddChild(childEnv);
					indentEnv->AddChild(noIndent);
				}
				else
				{
					indentEnv->AddChild(childEnv);
				}

				break;
			}
		case LuaAstNodeType::EmptyStatement:
			{
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
			{
				env->Add<KeyWordElement>(node);
				env->Add<KeepElement>(1);
				break;
			}
		case LuaAstNodeType::GeneralOperator:
			{
				// 基于这样的考虑 可能local 语句没有等号所以nameDefList的空格移上来
				if (node->GetTokenType() == '=')
				{
					env->Add<SpaceElement>(1);
				}

				env->Add<OperatorElement>(node);
				env->Add<KeepElement>(1);
				break;
			}
		case LuaAstNodeType::NameDefList:
			{
				env->AddChild(FormatNameDefList(node));
				break;
			}
		case LuaAstNodeType::ExpressionList:
			{
				std::shared_ptr<FormatElement> layout = nullptr;
				if (_options.local_assign_continuation_align_to_first_expression)
				{
					bool canAligned = true;
					// 但是如果表达式列表中出现跨行表达式,则采用长表达式对齐
					for (auto& expression : node->GetChildren())
					{
						if (expression->GetType() == LuaAstNodeType::Expression)
						{
							auto startLine = _parser->GetLine(expression->GetTextRange().StartOffset);
							auto endLine = _parser->GetLine(expression->GetTextRange().EndOffset);

							if (startLine != endLine)
							{
								canAligned = false;
								break;
							}
						}
					}
					if (canAligned)
					{
						layout = std::make_shared<AlignToFirstElement>();
					}
				}

				env->AddChild(FormatExpressionList(node, layout));
				break;
			}
		case LuaAstNodeType::Comment:
			{
				env->AddChild(FormatComment(node));
				env->Add<KeepElement>(1);
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
				env->Add<OperatorElement>(node);
				env->Add<KeepElement>(1);
				break;
			}
		case LuaAstNodeType::ExpressionList:
			{
				if (isLeftExprList)
				{
					env->AddChild(FormatAssignLeftExpressionList(node));
					env->Add<SpaceElement>(1);
					isLeftExprList = false;
				}
				else
				{
					std::shared_ptr<FormatElement> layout = nullptr;
					if (_options.local_assign_continuation_align_to_first_expression)
					{
						bool canAligned = true;
						// 但是如果表达式列表中出现跨行表达式,则采用长表达式对齐
						for (auto& expression : node->GetChildren())
						{
							if (expression->GetType() == LuaAstNodeType::Expression)
							{
								auto startLine = _parser->GetLine(expression->GetTextRange().StartOffset);
								auto endLine = _parser->GetLine(expression->GetTextRange().EndOffset);

								if (startLine != endLine)
								{
									canAligned = false;
									break;
								}
							}
						}
						if (canAligned)
						{
							layout = std::make_shared<AlignToFirstElement>();
						}
					}

					env->AddChild(FormatExpressionList(node, layout));
				}

				break;
			}
		case LuaAstNodeType::Comment:
			{
				env->AddChild(FormatNode(node));
				env->Add<KeepElement>(1);
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

	for (auto& node : nameDefList->GetChildren())
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
				env->Add<OperatorElement>(node);
				env->Add<KeepElement>(1);
				break;
			}
		case LuaAstNodeType::Comment:
			{
				env->AddChild(FormatComment(node));
				env->Add<KeepElement>(1);
				break;
			}
		case LuaAstNodeType::Attribute:
			{
				if (_options.keep_one_space_between_namedef_and_attribute)
				{
					env->Add<SpaceElement>(1);
				}

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
std::shared_ptr<FormatElement> LuaFormatter::FormatExpressionList(std::shared_ptr<LuaAstNode> expressionList,
                                                                  std::shared_ptr<FormatElement> env)
{
	if (env == nullptr)
	{
		env = std::make_shared<LongExpressionLayoutElement>(_options.continuation_indent_size);
	}

	for (auto& node : expressionList->GetChildren())
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
				env->Add<OperatorElement>(node);
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

std::shared_ptr<FormatElement> LuaFormatter::FormatCallArgsExpressionList(std::shared_ptr<LuaAstNode> expressionList,
                                                                          std::shared_ptr<FormatElement> env)
{
	if (env == nullptr)
	{
		env = std::make_shared<LongExpressionLayoutElement>(_options.continuation_indent_size);
	}

	auto& children = expressionList->GetChildren();
	for (auto& node : children)
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
				if (_options.remove_expression_list_finish_comma && node == children.back())
				{
					break;
				}
				env->Add<OperatorElement>(node);
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
	auto env = std::make_shared<LongExpressionLayoutElement>(_options.continuation_indent_size);

	for (auto& node : expressionList->GetChildren())
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
				env->Add<OperatorElement>(node);
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
	auto text = comment->GetText();
	if (!text.empty() && text.back() > 0 && ::isspace(text.back()) == 0)
	{
		return std::make_shared<TextElement>(comment);
	}
	int i = static_cast<int>(text.size()) - 1;
	for (; i >= 0; i--)
	{
		char ch = text[i];
		if (ch <= 0 || ::isspace(ch) == 0)
		{
			break;
		}
	}

	text = text.substr(0, i + 1);

	return std::make_shared<TextElement>(text, comment->GetTextRange());
}

std::shared_ptr<FormatElement> LuaFormatter::FormatBreakStatement(std::shared_ptr<LuaAstNode> breakNode)
{
	auto env = std::make_shared<StatementElement>();

	for (auto& child : breakNode->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->Add<KeyWordElement>(child);
				break;
			}
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<SpaceElement>(0);
				env->Add<OperatorElement>(child);
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

	for (auto& child : returnNode->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->Add<KeyWordElement>(child);
				break;
			}
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<SpaceElement>(0);
				env->Add<OperatorElement>(child);
				break;
			}
		case LuaAstNodeType::ExpressionList:
			{
				env->Add<SpaceElement>(1);
				env->AddChild(FormatNode(child));
				break;
			}
		case LuaAstNodeType::Comment:
			{
				env->AddChild(FormatComment(child));
				env->Add<KeepElement>(1);
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

	for (auto& child : gotoNode->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->Add<KeyWordElement>(child);
				env->Add<SpaceElement>(1);
				break;
			}
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<SpaceElement>(0);
				env->Add<OperatorElement>(child);
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

std::shared_ptr<FormatElement> LuaFormatter::FormatLabelStatement(std::shared_ptr<LuaAstNode> labelStatement)
{
	auto env = std::make_shared<StatementElement>();

	for (auto child : labelStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<OperatorElement>(child);
				break;
			}
		case LuaAstNodeType::Identify:
			{
				env->Add<SpaceElement>(0);
				env->AddChild(FormatNode(child));
				env->Add<SpaceElement>(0);
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

std::shared_ptr<FormatElement> LuaFormatter::FormatNameIdentify(std::shared_ptr<LuaAstNode> nameIdentify)
{
	return std::make_shared<TextElement>(nameIdentify);
}

std::shared_ptr<FormatElement> LuaFormatter::FormatGeneralOperator(std::shared_ptr<LuaAstNode> general)
{
	return std::make_shared<OperatorElement>(general);
}

std::shared_ptr<FormatElement> LuaFormatter::FormatKeyWords(std::shared_ptr<LuaAstNode> keywords)
{
	return std::make_shared<KeyWordElement>(keywords);
}

std::shared_ptr<FormatElement> LuaFormatter::FormatDoStatement(std::shared_ptr<LuaAstNode> doStatement)
{
	std::shared_ptr<FormatElement> env = std::make_shared<StatementElement>();
	auto& children = doStatement->GetChildren();
	auto it = children.begin();
	bool singleLine = false;
	auto doBlockEnd = FormatNodeAndBlockOrEnd(it, singleLine, children);

	if (_options.do_statement_no_indent && !singleLine)
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

std::shared_ptr<FormatElement> LuaFormatter::FormatWhileStatement(std::shared_ptr<LuaAstNode> whileStatement)
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
				if (child->GetTokenType() == TK_DO)
				{
					bool singleLine = false;
					env->AddChild(FormatNodeAndBlockOrEnd(it, singleLine, children));
					env->Add<KeepElement>(1);
				}
				else if (child->GetTokenType() == TK_WHILE)
				{
					env->Add<KeyWordElement>(child);
					env->Add<SpaceElement>(1);
				}
				else
				{
					env->Add<KeyWordElement>(child);
				}
				break;
			}
		case LuaAstNodeType::Expression:
			{
				auto expression = FormatNode(child);
				env->AddChild(expression);
				env->Add<SpaceElement>(1);
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
	for (auto& child : forStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->Add<KeyWordElement>(child);
				env->Add<SpaceElement>(1);
				break;
			}
		case LuaAstNodeType::ForNumber:
			{
				for (auto& forNumberChild : child->GetChildren())
				{
					switch (forNumberChild->GetType())
					{
					case LuaAstNodeType::Identify:
						{
							env->Add<TextElement>(forNumberChild);
							env->Add<SpaceElement>(1);
							break;
						}
					case LuaAstNodeType::GeneralOperator:
						{
							env->Add<OperatorElement>(forNumberChild);
							env->Add<SpaceElement>(1);
							break;
						}
					case LuaAstNodeType::Expression:
						{
							env->AddChild(FormatNode(forNumberChild));
							break;
						}
					case LuaAstNodeType::ForBody:
						{
							env->Add<SpaceElement>(1);
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
				for (auto& forListChild : child->GetChildren())
				{
					switch (forListChild->GetType())
					{
					case LuaAstNodeType::KeyWord:
						{
							env->Add<KeyWordElement>(forListChild);
							env->Add<SpaceElement>(1);
							break;
						}
					case LuaAstNodeType::NameDefList:
					case LuaAstNodeType::ExpressionList:
						{
							env->AddChild(FormatNode(forListChild));
							env->Add<SpaceElement>(1);
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
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		const auto child = *it;
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				if (child->GetTokenType() == TK_DO)
				{
					bool singleLine = false;
					env->AddChild(FormatNodeAndBlockOrEnd(it, singleLine, children));
					env->Add<KeepLineElement>();
				}
				else
				{
					env->Add<KeyWordElement>(child);
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

std::shared_ptr<FormatElement> LuaFormatter::FormatAttribute(std::shared_ptr<LuaAstNode> attribute)
{
	auto env = std::make_shared<ExpressionElement>();

	for (auto& child : attribute->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::Identify:
			{
				env->Add<SpaceElement>(0);
				env->Add<TextElement>(child);
				env->Add<SpaceElement>(0);
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

std::shared_ptr<FormatElement> LuaFormatter::FormatRepeatStatement(std::shared_ptr<LuaAstNode> repeatStatement)
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
				if (child->GetTokenType() == TK_REPEAT)
				{
					bool singleLine = false;
					env->AddChild(FormatNodeAndBlockOrEnd(it, singleLine, children));
					if (singleLine)
					{
						env->Add<KeepElement>(1);
					}
					else
					{
						env->Add<KeepLineElement>();
					}
				}
				else
				{
					env->Add<KeyWordElement>(child);
					env->Add<SpaceElement>(1);
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

	// std::vector<std::shared_ptr<PlaceholderElement>> placeholderExpressions;

	for (auto it = children.begin(); it != children.end(); ++it)
	{
		const auto child = *it;
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				if (child->GetTokenType() == TK_THEN || child->GetTokenType() == TK_ELSE)
				{
					bool singleLine = false;
					env->AddChild(FormatNodeAndBlockOrEnd(it, singleLine, children));
					env->Add<KeepElement>(1, !singleLine);
				}
				else if (child->GetTokenType() == TK_IF || child->GetTokenType() == TK_ELSEIF)
				{
					env->Add<KeyWordElement>(child);
					env->Add<SpaceElement>(1);
				}
				else // 然而end是在 FormatNodeAndBlockOrEnd 中完成的
				{
					env->Add<KeyWordElement>(child);
				}

				break;
			}
		case LuaAstNodeType::Expression:
			{
				std::shared_ptr<FormatElement> expression = nullptr;
				if (_options.if_condition_no_continuation_indent
					|| _options.if_condition_align_with_each_other)
				{
					expression = std::make_shared<LongExpressionLayoutElement>(0);
				}

				env->AddChild(FormatExpression(child, expression));
				env->Add<KeepElement>(1);
				break;
			}
		default:
			{
				DefaultHandle(child, env);
				env->Add<KeepElement>(1);
				break;
			}
		}
	}

	if (_options.if_condition_align_with_each_other)
	{
		auto ifAlignLayout = std::make_shared<AlignIfElement>();
		ifAlignLayout->CopyFrom(env);
		env->Reset();
		env->AddChild(ifAlignLayout);
	}

	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatExpressionStatement(std::shared_ptr<LuaAstNode> expressionStatement)
{
	auto env = std::make_shared<StatementElement>();

	for (auto& child : expressionStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::CallExpression:
			{
				env->AddChild(FormatCallExpression(child));
				break;
			}
		case LuaAstNodeType::Expression:
			{
				std::shared_ptr<FormatElement> expressionEnv = nullptr;
				if (_options.align_chained_expression_statement)
				{
					auto indexExpression = ast_util::FindLeftIndexExpression(child);
					if (indexExpression)
					{
						auto indexOp = indexExpression->FindFirstOf(LuaAstNodeType::IndexOperator);
						if (indexOp->GetTokenType() == '.' || indexOp->GetTokenType() == ':')
						{
							auto continuationIndent = _parser->GetColumn(indexOp->GetTextRange().StartOffset)
								- _parser->GetColumn(child->GetTextRange().StartOffset);
							expressionEnv = std::make_shared<LongExpressionLayoutElement>(continuationIndent);
						}
					}
				}
				env->AddChild(FormatExpression(child, expressionEnv));
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
	const auto& children = callArgList->GetChildren();

	std::vector<std::shared_ptr<LuaAstNode>> argList;
	if ((ast_util::IsSingleStringOrTableArg(callArgList)
			&& _options.call_arg_parentheses == CallArgParentheses::Remove)
		|| (_options.call_arg_parentheses == CallArgParentheses::RemoveTableOnly
			&& ast_util::IsSingleTableArg(callArgList))
		|| (_options.call_arg_parentheses == CallArgParentheses::RemoveStringOnly
			&& ast_util::IsSingleStringArg(callArgList))
		|| (_options.call_arg_parentheses == CallArgParentheses::UnambiguousRemoveStringOnly
			&& ast_util::IsSingleStringArgUnambiguous(callArgList)))
	{
		for (auto child : children)
		{
			if (child->GetType() != LuaAstNodeType::GeneralOperator)
			{
				argList.push_back(child);
			}
		}
	}
	else
	{
		argList = children;
	}

	for (auto child : argList)
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::ExpressionList:
			{
				std::shared_ptr<FormatElement> layout = nullptr;
				if (_options.align_call_args != AlignCallArgs::False)
				{
					bool canAligned = true;
					// 但是如果表达式列表中出现跨行表达式,则采用长表达式对齐
					for (auto& expression : child->GetChildren())
					{
						if (expression->GetType() == LuaAstNodeType::Expression)
						{
							auto startLine = _parser->GetLine(expression->GetTextRange().StartOffset);
							auto endLine = _parser->GetLine(expression->GetTextRange().EndOffset);

							if (startLine != endLine)
							{
								if (!expression->GetChildren().empty())
								{
									auto first = expression->GetChildren().front();
									if (first->GetType() == LuaAstNodeType::ClosureExpression
										|| first->GetType() == LuaAstNodeType::TableExpression
										|| _options.align_call_args == AlignCallArgs::OnlyNotExistCrossExpression)
									{
										canAligned = false;
										break;
									}
								}
							}
						}
					}

					if (canAligned
						&& _options.align_call_args == AlignCallArgs::OnlyAfterMoreIndentionStatement
						&& (!ast_util::IsNodeAfterMoreIndentionStatement(child))
					)
					{
						canAligned = false;
					}

					if (canAligned)
					{
						layout = std::make_shared<AlignToFirstElement>();
					}
				}
				else
				{
					layout = std::make_shared<CallArgsListLayoutElement>();
				}

				env->AddChild(FormatCallArgsExpressionList(child, layout));
				env->Add<KeepElement>(0);
				break;
			}
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<OperatorElement>(child);
				env->Add<KeepElement>(0);
				break;
			}
		case LuaAstNodeType::StringLiteralExpression:
		case LuaAstNodeType::TableExpression:
			{
				auto layout = std::make_shared<CallArgsListLayoutElement>();
				layout->AddChild(FormatNode(child));
				env->AddChild(layout);
				env->Add<KeepElement>(0);
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

	for (auto& child : functionStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->Add<KeyWordElement>(child);
				env->Add<SpaceElement>(1);
				break;
			}
		case LuaAstNodeType::NameExpression:
			{
				env->AddChild(FormatNameExpression(child));
				break;
			}
		case LuaAstNodeType::FunctionBody:
			{
				if (_options.space_before_function_open_parenthesis)
				{
					env->Add<SpaceElement>(1);
				}
				else
				{
					env->Add<SpaceElement>(0);
				}

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

std::shared_ptr<FormatElement> LuaFormatter::FormatNameExpression(std::shared_ptr<LuaAstNode> nameExpression)
{
	auto env = std::make_shared<ExpressionElement>();

	for (auto& child : nameExpression->GetChildren())
	{
		FormatSubExpression(child, env);
	}

	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatParamList(std::shared_ptr<LuaAstNode> paramList)
{
	auto env = std::make_shared<ExpressionElement>();

	std::shared_ptr<FormatElement> paramListLayoutEnv = nullptr;

	if (_options.align_function_define_params)
	{
		paramListLayoutEnv = std::make_shared<AlignToFirstElement>();
	}
	else
	{
		paramListLayoutEnv = std::make_shared<LongExpressionLayoutElement>(_options.continuation_indent_size);
	}

	auto& children = paramList->GetChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		const auto child = *it;
		switch (child->GetType())
		{
		case LuaAstNodeType::GeneralOperator:
			{
				if (child->GetTokenType() == ',')
				{
					if (_options.remove_expression_list_finish_comma
						&& NextMatch(it, LuaAstNodeType::GeneralOperator, children))
					{
						break;
					}
					paramListLayoutEnv->Add<OperatorElement>(child);
					paramListLayoutEnv->Add<KeepElement>(1);
				}
				else if (child->GetTokenType() == ')')
				{
					env->AddChild(paramListLayoutEnv);
					if (!paramListLayoutEnv->GetChildren().empty())
					{
						env->Add<KeepElement>(0);
					}

					env->Add<OperatorElement>(child);
				}
				else
				{
					env->Add<OperatorElement>(child);
					env->Add<KeepElement>(0);
				}
				break;
			}
		case LuaAstNodeType::Param:
			{
				paramListLayoutEnv->Add<TextElement>(child);
				if (NextMatch(it, LuaAstNodeType::Comment, children))
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
				paramListLayoutEnv->AddChild(FormatComment(child));
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
	auto& children = functionBody->GetChildren();
	auto it = children.begin();
	bool singleLine = false;
	return FormatNodeAndBlockOrEnd(it, singleLine, children);
}


std::shared_ptr<FormatElement> LuaFormatter::FormatClosureExpression(std::shared_ptr<LuaAstNode> closureExpression)
{
	auto env = std::make_shared<ExpressionElement>();

	for (auto& child : closureExpression->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->Add<KeyWordElement>(child);
				break;
			}
		case LuaAstNodeType::FunctionBody:
			{
				if (_options.space_before_function_open_parenthesis)
				{
					env->Add<SpaceElement>(1);
				}
				else
				{
					env->Add<SpaceElement>(0);
				}

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

std::shared_ptr<FormatElement> LuaFormatter::FormatLocalFunctionStatement(
	std::shared_ptr<LuaAstNode> localFunctionStatement)
{
	auto env = std::make_shared<StatementElement>();

	for (auto& child : localFunctionStatement->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::KeyWord:
			{
				env->Add<KeyWordElement>(child);
				env->Add<SpaceElement>(1);
				break;
			}
		case LuaAstNodeType::Identify:
			{
				env->Add<TextElement>(child);
				env->Add<SpaceElement>(0);
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

	std::shared_ptr<FormatElement> tableFieldLayout = nullptr;
	if (_options.align_table_field_to_first_field)
	{
		tableFieldLayout = std::make_shared<AlignToFirstElement>();
	}
	else
	{
		tableFieldLayout = std::make_shared<IndentOnLineBreakElement>();
	}

	int leftBraceLine = 0;
	bool forceChopDownEndBracket = false;

	for (auto it = children.begin(); it != children.end(); ++it)
	{
		const auto child = *it;
		switch (child->GetType())
		{
		case LuaAstNodeType::GeneralOperator:
			{
				if (child->GetTokenType() == '{')
				{
					env->Add<OperatorElement>(child);
					leftBraceLine = _parser->GetLine(child->GetTextRange().EndOffset);
					auto next = NextNode(it, children);
					if (next && next->GetTokenType() != '}')
					{
						forceChopDownEndBracket = leftBraceLine != _parser->GetLine(next->GetTextRange().StartOffset);
					}
				}
				else if (child->GetTokenType() == '}')
				{
					if (tableFieldLayout->GetChildren().empty())
					{
						env->Add<KeepElement>(0);
					}
					else
					{
						env->Add<KeepElement>(_options.keep_one_space_between_table_and_bracket ? 1 : 0);
						env->AddChild(tableFieldLayout);

						if (forceChopDownEndBracket)
						{
							if (_parser->GetLine(tableFieldLayout->GetTextRange().EndOffset) != _parser->GetLine(
								child->GetTextRange().StartOffset))
							{
								env->Add<KeepElement>(1);
							}
							else
							{
								env->Add<LineElement>();
							}
						}
						else
						{
							env->Add<KeepElement>(_options.keep_one_space_between_table_and_bracket ? 1 : 0);
						}
						tableFieldLayout = nullptr;
					}
					env->Add<OperatorElement>(child);
				}

				break;
			}
		default:
			{
				if (tableFieldLayout)
				{
					auto fields = FormatAlignTableField(it, leftBraceLine, children);
					if (fields->Is(FormatElementType::ExpressionElement))
					{
						tableFieldLayout->AddChildren(fields->GetChildren());
					}
					else
					{
						tableFieldLayout->AddChild(fields);
					}
					tableFieldLayout->Add<KeepElement>(1);
				}
				else
				{
					DefaultHandle(child, env);
				}
			}
		}
	}
	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatTableField(std::shared_ptr<LuaAstNode> tableField)
{
	auto env = std::make_shared<ExpressionElement>();
	auto eqSignFounded = false;
	auto& children = tableField->GetChildren();
	bool isIndexExprLongString = false;
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		auto& child = *it;
		switch (child->GetType())
		{
		case LuaAstNodeType::GeneralOperator:
			{
				if (child->GetTokenType() == '=')
				{
					eqSignFounded = true;
					env->Add<SpaceElement>(1);
					env->Add<OperatorElement>(child);
					env->Add<KeepElement>(1);
				}
				else
				{
					env->Add<OperatorElement>(child);
				}
				break;
			}
		case LuaAstNodeType::IndexOperator:
			{
				if (child->GetTokenType() == '[')
				{
					auto nextNode = NextNode(it, children);
					if (nextNode && nextNode->GetType() == LuaAstNodeType::Expression
						&& ast_util::WillIndexExpressionFormatError(nextNode))
					{
						isIndexExprLongString = true;
						env->Add<OperatorElement>(child);
						env->Add<KeepElement>(1);
						continue;
					}
				}
				else if (child->GetTokenType() == ']' && isIndexExprLongString)
				{
					env->Add<KeepElement>(1);
				}

				env->Add<OperatorElement>(child);
				break;
			}
		case LuaAstNodeType::Identify:
			{
				env->Add<TextElement>(child);
				break;
			}
		case LuaAstNodeType::Comment:
			{
				env->AddChild(FormatComment(child));
				env->Add<KeepElement>(1);
				break;
			}
		case LuaAstNodeType::Expression:
			{
				std::shared_ptr<FormatElement> layout = nullptr;
				if (_options.table_field_continuation_align_to_first_sub_expression && eqSignFounded)
				{
					bool canAligned = true;
					// 但是如果表达式列表中出现跨行表达式,则采用长表达式对齐
					for (auto& expression : child->GetChildren())
					{
						if (expression->GetType() == LuaAstNodeType::Expression)
						{
							auto startLine = _parser->GetLine(expression->GetTextRange().StartOffset);
							auto endLine = _parser->GetLine(expression->GetTextRange().EndOffset);

							if (startLine != endLine)
							{
								canAligned = false;
								break;
							}
						}
					}
					if (canAligned)
					{
						layout = std::make_shared<AlignToFirstElement>();
					}
				}

				env->AddChild(FormatExpression(child, layout));
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

std::shared_ptr<FormatElement> LuaFormatter::FormatStringLiteralExpression(
	std::shared_ptr<LuaAstNode> stringLiteralExpression)
{
	return std::make_shared<StringLiteralElement>(stringLiteralExpression);
}

std::shared_ptr<FormatElement> LuaFormatter::FormatTableSep(std::shared_ptr<LuaAstNode> tableSep)
{
	return std::make_shared<SepElement>(tableSep);
}

void LuaFormatter::DefaultHandle(std::shared_ptr<LuaAstNode> node, std::shared_ptr<FormatElement> envElement)
{
	envElement->AddChild(FormatNode(node));
}

std::shared_ptr<FormatElement> LuaFormatter::FormatAlignStatement(LuaAstNode::ChildIterator& it,
                                                                  const LuaAstNode::ChildrenContainer& children)
{
	std::shared_ptr<FormatElement> env = nullptr;
	if (_options.continuous_assign_statement_align_to_equal_sign)
	{
		env = std::make_shared<AlignmentLayoutElement>('=');
	}
	else
	{
		env = std::make_shared<ExpressionElement>();
	}

	env->AddChild(FormatNode(*it));

	auto nextChild = NextNode(it, children);
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
		if (nextLine - currentLine > _options.max_continuous_line_distance)
		{
			break;
		}

		if (nextChild->GetType() == LuaAstNodeType::Comment && ast_util::IsTagFormat(nextChild))
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
				lastStatementEnv->TrimEnd();
				lastStatementEnv->Add<SpaceElement>(_options.statement_inline_comment_space);
				lastStatementEnv->AddChild(FormatNode(nextChild));
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
				auto comment = FormatComment(nextChild);
				auto commentStatement = std::make_shared<StatementElement>();
				commentStatement->AddChild(comment);
				env->AddChild(commentStatement);
			}
			else
			{
				env->AddChild(FormatNode(nextChild));
			}
		}

		++it;

		nextChild = NextNode(it, children);
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

std::shared_ptr<FormatElement> LuaFormatter::FormatAlignTableField(LuaAstNode::ChildIterator& it,
                                                                   int leftBraceLine,
                                                                   const LuaAstNode::ChildrenContainer& siblings)
{
	bool canAlign = true;
	std::shared_ptr<FormatElement> layout = std::make_shared<ExpressionElement>();
	if (leftBraceLine == _parser->GetLine((*it)->GetTextRange().StartOffset))
	{
		canAlign = false;
	}
	std::shared_ptr<LuaAstNode> current = nullptr;
	std::shared_ptr<LuaAstNode> nextSibling = nullptr;

	for (; it != siblings.end(); ++it)
	{
		current = *it;
		nextSibling = NextNode(it, siblings);

		// unreachable
		if (nextSibling == nullptr)
		{
			layout->AddChild(FormatNode(current));
			return layout;
		}

		if (nextSibling->GetType() == LuaAstNodeType::TableField
			|| nextSibling->GetType() == LuaAstNodeType::TableFieldSep
			|| nextSibling->GetType() == LuaAstNodeType::Comment)
		{
			int currentLine = _parser->GetLine(current->GetTextRange().EndOffset);
			int nextLine = _parser->GetLine(nextSibling->GetTextRange().StartOffset);

			if (nextLine == currentLine)
			{
				// 检查是否会是内联注释
				// 比如 t = 123, -- inline comment
				// 或者 c = 456 --fff
				// 或者 ddd = 123 --[[ffff]] ,
				if ((current->GetType() == LuaAstNodeType::TableField
						|| current->GetType() == LuaAstNodeType::TableFieldSep)
					&& nextSibling->GetType() == LuaAstNodeType::Comment)
				{
					layout->AddChild(FormatNode(current));
					layout->Add<SpaceElement>(1);
				}
				else if (current->GetType() == LuaAstNodeType::TableFieldSep
					&& nextSibling->GetType() == LuaAstNodeType::TableField)
				{
					canAlign = false;
					layout->AddChild(FormatNode(current));
					// 此时认为table 不应该考虑对齐到等号
					layout->Add<KeepElement>(1);
				}
				else if (current->GetType() == LuaAstNodeType::TableField
					&& nextSibling->GetType() == LuaAstNodeType::TableFieldSep)
				{
					layout->AddChild(FormatNode(current));
					layout->Add<SpaceElement>(0);
				}
				else
				{
					layout->AddChild(FormatNode(current));
					layout->Add<KeepElement>(1);
				}
			}
			else if (currentLine == leftBraceLine)
			{
				layout->AddChild(FormatNode(current));
				break;
			}
			else if (nextLine - currentLine <= _options.max_continuous_line_distance)
			{
				layout->AddChild(FormatNode(current));
				layout->Add<KeepElement>(1);
			}
			else
			{
				layout->AddChild(FormatNode(current));
				break;
			}
		}
		else if (nextSibling->GetType() == LuaAstNodeType::GeneralOperator)
		{
			layout->AddChild(FormatNode(current));
			break;
		}
	}

	// table field finish
	if (nextSibling != nullptr
		&& nextSibling->GetTokenType() == '}'
		&& _options.trailing_table_separator != TrailingTableSeparator::Keep)
	{
		// find table field
		auto& layoutChildren = layout->GetChildren();
		auto rIt = layoutChildren.rbegin();
		for (; rIt != layoutChildren.rend(); rIt++)
		{
			auto child = *rIt;
			// table field
			if (child->Is(FormatElementType::ExpressionElement))
			{
				break;
			}
		}

		// if founded
		if (rIt != layoutChildren.rend())
		{
			std::shared_ptr<SepElement> sep = nullptr;
			auto sepIt = rIt.base();

			for (; sepIt != layoutChildren.end(); sepIt++)
			{
				auto child = *sepIt;
				if (child->HasValidTextRange())
				{
					if (child->Is(FormatElementType::SepElement))
					{
						sep = std::dynamic_pointer_cast<SepElement>(child);
					}
					break;
				}
			}

			switch (_options.trailing_table_separator)
			{
			case TrailingTableSeparator::Never:
				{
					if (sep)
					{
						sep->SetEmpty();
					}
					break;
				}
			case TrailingTableSeparator::Always:
				{
					if (!sep)
					{
						layoutChildren.insert(rIt.base(), SepElement::After((*rIt)));
					}

					break;
				}
			case TrailingTableSeparator::Smart:
				{
					auto line = _parser->GetLine((*rIt)->GetTextRange().EndOffset);
					if (line == leftBraceLine)
					{
						// 不要合并上去
						if (sep)
						{
							sep->SetEmpty();
						}
					}
					else if (!sep)
					{
						layoutChildren.insert(rIt.base(), SepElement::After((*rIt)));
					}

					break;
				}
			default:
				{
					break;
				}
			}
		}
	}


	if (canAlign && _options.continuous_assign_table_field_align_to_equal_sign && layout->GetChildren().size() > 1)
	{
		auto alignmentLayoutElement = std::make_shared<AlignmentLayoutElement>('=');
		alignmentLayoutElement->CopyFrom(layout);
		layout = alignmentLayoutElement;
	}

	return layout;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatNodeAndBlockOrEnd(LuaAstNode::ChildIterator& it,
                                                                     bool& singleLineBlock,
                                                                     const LuaAstNode::ChildrenContainer& children)
{
	auto env = std::make_shared<ExpressionElement>();
	auto keyNode = *it;
	auto parentNode = keyNode->GetParent();
	env->AddChild(FormatNode(keyNode));

	if (NextMatch(it, LuaAstNodeType::Comment, children))
	{
		auto comment = NextNode(it, children);
		int currentLine = _parser->GetLine(keyNode->GetTextRange().EndOffset);
		int nextLine = _parser->GetLine(comment->GetTextRange().StartOffset);

		// 认为是内联注释
		if (nextLine == currentLine)
		{
			env->Add<SpaceElement>(1);
			env->AddChild(FormatComment(comment));
			++it;
		}
	}

	bool blockExist = false;
	auto block = FormatBlockFromParent(it, children);

	if (!block->GetChildren().empty())
	{
		blockExist = true;
		if (_parser->GetLine(keyNode->GetTextRange().StartOffset) != _parser->GetLine(block->GetTextRange().EndOffset))
		{
			if (_options.remove_empty_header_and_footer_lines_in_function
				&& parentNode && parentNode->GetType() == LuaAstNodeType::FunctionBody)
			{
				env->Add<LineElement>();
				env->AddChild(block);
			}
			else
			{
				env->Add<KeepElement>(1);
				env->AddChild(block);
				env->Add<KeepElement>(1, true);
			}
		}
		else
		{
			singleLineBlock = true;
			env->Add<KeepElement>(1);

			for (auto blockChild : block->GetChildren())
			{
				if (blockChild->HasValidTextRange())
				{
					auto shortExpression = std::make_shared<ExpressionElement>();
					shortExpression->AddChildren(blockChild->GetChildren());
					env->AddChild(shortExpression);
					env->Add<KeepElement>(1);
				}
			}
		}
	}
	else
	{
		env->Add<KeepElement>(1);
	}

	if (NextMatch(it, LuaAstNodeType::KeyWord, children))
	{
		auto next = NextNode(it, children);
		if (next->GetTokenType() == TK_END)
		{
			env->Add<TextElement>(next);
			++it;
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

std::shared_ptr<FormatElement> LuaFormatter::FormatBlockFromParent(LuaAstNode::ChildIterator& it,
                                                                   const LuaAstNode::ChildrenContainer& siblings)
{
	std::shared_ptr<LuaAstNode> block = nullptr;
	std::vector<std::shared_ptr<LuaAstNode>> comments;
	std::vector<std::shared_ptr<LuaAstNode>> afterBlockComments;
	for (; it != siblings.end(); ++it)
	{
		if (NextMatch(it, LuaAstNodeType::Comment, siblings))
		{
			auto next = NextNode(it, siblings);
			if (block)
			{
				if (_parser->GetLine(block->GetTextRange().EndOffset) != _parser->GetLine(
					next->GetTextRange().StartOffset))
				{
					afterBlockComments.push_back(next);
					continue;
				}
			}

			comments.push_back(next);
		}
		else if (NextMatch(it, LuaAstNodeType::Block, siblings))
		{
			block = NextNode(it, siblings);
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

		auto nextKey = NextNode(it, siblings);
		if (_options.if_branch_comments_after_block_no_indent
			&& nextKey && nextKey->GetType() == LuaAstNodeType::KeyWord
			&& (nextKey->GetTokenType() == TK_ELSEIF || nextKey->GetTokenType() == TK_ELSE))
		{
			auto blockEnv = FormatBlock(copyBlock);
			auto noIndentEnv = std::make_shared<NoIndentElement>();
			for (auto comment : afterBlockComments)
			{
				auto commentStatement = std::make_shared<StatementElement>();
				commentStatement->AddChild(FormatComment(comment));
				noIndentEnv->AddChild(commentStatement);
				noIndentEnv->Add<KeepLineElement>();
			}
			blockEnv->AddChild(noIndentEnv);

			return blockEnv;
		}
		else
		{
			for (auto comment : afterBlockComments)
			{
				copyBlock->AddComment(comment);
			}
			return FormatBlock(copyBlock);
		}
	}
	else
	{
		auto indentElement = std::make_shared<IndentElement>();
		for (auto comment : comments)
		{
			auto commentStatement = std::make_shared<StatementElement>();
			commentStatement->AddChild(FormatComment(comment));
			indentElement->AddChild(commentStatement);
			indentElement->Add<KeepLineElement>();
		}

		return indentElement;
	}
}


void LuaFormatter::FormatSubExpression(std::shared_ptr<LuaAstNode> expression,
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
		env = std::make_shared<LongExpressionLayoutElement>(_options.continuation_indent_size);
	}

	auto& children = expression->GetChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		const auto current = *it;

		FormatSubExpression(current, env);
		env->Add<KeepElement>(0);
	}

	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatBinaryExpression(std::shared_ptr<LuaAstNode> binaryExpression)
{
	auto env = std::make_shared<SubExpressionElement>();
	auto& children = binaryExpression->GetChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		const auto child = *it;
		switch (child->GetType())
		{
		case LuaAstNodeType::BinaryOperator:
			{
				env->Add<OperatorElement>(child);
				env->Add<KeepElement>(1);
				break;
			}
		default:
			{
				FormatSubExpression(child, env);
				env->Add<KeepElement>(1);
			}
		}
	}

	return env;
}

std::shared_ptr<FormatElement> LuaFormatter::FormatUnaryExpression(std::shared_ptr<LuaAstNode> unaryExpression)
{
	auto env = std::make_shared<SubExpressionElement>();
	auto& children = unaryExpression->GetChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		auto child = *it;
		switch (child->GetType())
		{
		case LuaAstNodeType::UnaryOperator:
			{
				env->Add<OperatorElement>(child);
				if (child->GetTokenType() == TK_NOT)
				{
					env->Add<KeepElement>(1);
				}
				else
				{
					auto next = NextNode(it, children);
					if (next && (next->GetType() == LuaAstNodeType::UnaryExpression || next->GetType() ==
						LuaAstNodeType::Comment))
					{
						env->Add<KeepElement>(1);
						break;
					}
					env->Add<KeepElement>(0);
				}
				break;
			}
		default:
			{
				FormatSubExpression(child, env);
				env->Add<KeepElement>(1);
			}
		}
	}

	return env;
}


std::shared_ptr<FormatElement> LuaFormatter::FormatPrimaryExpression(std::shared_ptr<LuaAstNode> primaryExpression)
{
	auto env = std::make_shared<SubExpressionElement>();
	for (auto& child : primaryExpression->GetChildren())
	{
		switch (child->GetType())
		{
		case LuaAstNodeType::GeneralOperator:
			{
				env->Add<OperatorElement>(child);
				env->Add<KeepElement>(0);
				break;
			}
		case LuaAstNodeType::Expression:
		case LuaAstNodeType::Identify:
		case LuaAstNodeType::NameIdentify:
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
	bool expressionAfterIndexOperator = false;
	bool isIndexExprLongString = false;
	auto env = std::make_shared<SubExpressionElement>();
	auto& children = indexExpression->GetChildren();
	for (auto it = children.begin(); it != children.end(); it++)
	{
		auto& child = *it;
		switch (child->GetType())
		{
		case LuaAstNodeType::IndexOperator:
			{
				if (child->GetTokenType() == '[')
				{
					expressionAfterIndexOperator = true;
					auto nextNode = NextNode(it, children);
					if (nextNode && nextNode->GetType() == LuaAstNodeType::Expression
						&& ast_util::WillIndexExpressionFormatError(nextNode))
					{
						isIndexExprLongString = true;
						env->Add<OperatorElement>(child);
						env->Add<KeepElement>(1);
						continue;
					}

					env->Add<OperatorElement>(child);
					env->Add<KeepElement>(0);
				}
				else if (child->GetTokenType() == ']')
				{
					env->Add<KeepElement>(isIndexExprLongString ? 1 : 0);
					env->Add<OperatorElement>(child);
					env->Add<KeepElement>(0);
				}
				else if (child->GetTokenType() == ':')
				{
					env->Add<OperatorElement>(child);
					if (_options.long_chain_expression_allow_one_space_after_colon)
					{
						if (_parser->GetLuaFile()->OnlyEmptyCharBefore(
							child->GetTextRange().StartOffset))
						{
							env->Add<MaxSpaceElement>(1);
							continue;
						}
					}
					env->Add<KeepElement>(0);
				}
				else
				{
					DefaultHandle(child, env);
				}

				break;
			}
		case LuaAstNodeType::Comment:
			{
				env->AddChild(FormatComment(child));
				env->Add<KeepElement>(1);
				break;
			}
		case LuaAstNodeType::Expression:
			{
				if (_options.table_append_expression_no_space && expressionAfterIndexOperator)
				{
					auto text = child->GetText();
					if (StringUtil::StartWith(text, "#"))
					{
						env->AddChild(FormatTableAppendExpression(child));
						continue;
					}
				}

				FormatSubExpression(child, env);
				break;
			}
		default:
			{
				FormatSubExpression(child, env);

				auto nextNode = NextNode(it, children);
				if (_options.space_before_open_square_bracket
					&& nextNode != nullptr
					&& nextNode->GetType() == LuaAstNodeType::IndexOperator
					&& nextNode->GetTokenType() == '['
					&& !StringUtil::EndWith(child->GetText(), "]"))
				{
					env->Add<KeepElement>(1);
				}
				else
				{
					env->Add<KeepElement>(0);
				}
			}
		}
	}
	return env;
}


std::shared_ptr<FormatElement> LuaFormatter::FormatCallExpression(std::shared_ptr<LuaAstNode> callExpression)
{
	auto env = std::make_shared<SubExpressionElement>();
	auto& children = callExpression->GetChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		const auto child = *it;

		switch (child->GetType())
		{
		case LuaAstNodeType::IndexExpression:
		case LuaAstNodeType::PrimaryExpression:
			{
				FormatSubExpression(child, env);

				auto next = NextNode(it, children);
				if (next && next->GetType() == LuaAstNodeType::CallArgList)
				{
					if (!ast_util::WillCallArgHaveParentheses(next, _options.call_arg_parentheses) || _options.
						space_before_function_open_parenthesis)
					{
						//TODO workaround
						env->Add<KeepElement>(1, false, false);
					}
					else
					{
						env->Add<KeepElement>(0, false, false);
					}
				}
				else
				{
					env->Add<KeepElement>(1, false, false);
				}

				break;
			}
		case LuaAstNodeType::CallExpression:
			{
				env->AddChild(FormatNode(child));

				auto currentCallArgList = child->FindFirstOf(LuaAstNodeType::CallArgList);
				auto nextCallArgList = NextNode(it, children);
				bool needSpace = true;
				if (currentCallArgList && nextCallArgList)
				{
					bool currentHas = ast_util::WillCallArgHaveParentheses(
						currentCallArgList, _options.call_arg_parentheses);
					bool nextHas = ast_util::WillCallArgHaveParentheses(nextCallArgList, _options.call_arg_parentheses);

					if (currentHas && nextHas)
					{
						needSpace = false;
					}
				}
				else
				{
					needSpace = false;
				}

				env->Add<KeepElement>(needSpace ? 1 : 0, false, !nextCallArgList);

				break;
			}
		case LuaAstNodeType::CallArgList:
			{
				env->AddChild(FormatCallArgList(child));
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

std::shared_ptr<FormatElement> LuaFormatter::FormatTableAppendExpression(std::shared_ptr<LuaAstNode> expression)
{
	auto env = std::make_shared<LongExpressionLayoutElement>(_options.continuation_indent_size);

	if (expression->GetChildren().size() == 1)
	{
		auto root = expression->GetChildren().front();
		if (root->GetType() == LuaAstNodeType::BinaryExpression && root->GetChildren().size() == 3)
		{
			auto& binaryExpressionChildren = root->GetChildren();
			if (binaryExpressionChildren[0]->GetType() == LuaAstNodeType::UnaryExpression
				&& (binaryExpressionChildren[1]->GetType() == LuaAstNodeType::BinaryOperator
					&& binaryExpressionChildren[1]->GetText() == "+")
				&& binaryExpressionChildren[2]->GetText() == "1")
			{
				FormatSubExpression(binaryExpressionChildren[0], env);
				env->Add<KeepElement>(0);
				env->Add<TextElement>(binaryExpressionChildren[1]);
				env->Add<KeepElement>(0);
				env->Add<TextElement>(binaryExpressionChildren[2]);
				return env;
			}
		}
	}

	return FormatExpression(expression, env);
}

//
// std::shared_ptr<FormatElement> LuaFormatter::FormatRangeBlock(std::shared_ptr<LuaAstNode> blockNode,
//                                                               LuaFormatRange& validRange)
// {
// 	enum class State
// 	{
// 		UnReach,
// 		Contain,
// 	} state = State::UnReach;
//
// 	auto indentEnv = std::make_shared<IndentElement>();
//
// 	auto& statements = blockNode->GetChildren();
//
// 	for (auto it = statements.begin(); it != statements.end(); ++it)
// 	{
// 		const auto statement = *it;
//
// 		switch (state)
// 		{
// 		case State::UnReach:
// 			{
// 				TextRange textRange = statement->GetTextRange();
// 				if (statement->GetType() == LuaAstNodeType::AssignStatement
// 					|| statement->GetType() == LuaAstNodeType::LocalStatement
// 					|| statement->GetType() == LuaAstNodeType::Comment)
// 				{
// 					auto lastLine = _parser->GetLine(textRange.EndOffset);
// 					auto newIt = it;
// 					while (NextMatch(newIt, LuaAstNodeType::AssignStatement, statements)
// 						|| NextMatch(newIt, LuaAstNodeType::LocalStatement, statements)
// 						|| NextMatch(newIt, LuaAstNodeType::Comment, statements))
// 					{
// 						auto next = NextNode(newIt, statements);
// 						auto nextTextRange = next->GetTextRange();
//
// 						auto nextTextLine = _parser->GetLine(nextTextRange.StartOffset);
//
// 						if (nextTextLine - lastLine > 2)
// 						{
// 							break;
// 						}
// 						textRange.EndOffset = nextTextRange.EndOffset;
// 						++newIt;
// 					}
// 					auto statementEndLine = _parser->GetLine(textRange.EndOffset);
//
// 					if (statementEndLine >= validRange.StartLine)
// 					{
// 						state = State::Contain;
// 						break;
// 					}
// 					else
// 					{
// 						it = newIt;
// 						continue;
// 					}
// 				}
// 				else
// 				{
// 					auto statementEndLine = _parser->GetLine(textRange.EndOffset);
//
// 					if (statementEndLine >= validRange.StartLine)
// 					{
// 						state = State::Contain;
// 						break;
// 					}
// 					else
// 					{
// 						continue;
// 					}
// 				}
// 				break;
// 			}
// 		case State::Contain:
// 			{
// 				auto statementStartLine = _parser->GetLine(statement->GetTextRange().StartOffset);
// 				if (statementStartLine > validRange.EndLine)
// 				{
// 					goto endLoop;
// 				}
// 				break;
// 			}
// 		}
//
//
// 		switch (statement->GetType())
// 		{
// 		case LuaAstNodeType::AssignStatement:
// 		case LuaAstNodeType::LocalStatement:
// 			{
// 				if (NextMatch(it, LuaAstNodeType::AssignStatement, statements)
// 					|| NextMatch(it, LuaAstNodeType::LocalStatement, statements)
// 					|| NextMatch(it, LuaAstNodeType::Comment, statements))
// 				{
// 					indentEnv->AddChild(FormatAlignStatement(it, statements));
// 				}
// 				else
// 				{
// 					auto childEnv = FormatNode(statement);
// 					indentEnv->AddChild(childEnv);
// 				}
// 				indentEnv->AddChild(_options.keep_line_after_local_or_assign_statement);
// 				break;
// 			}
// 		case LuaAstNodeType::RepeatStatement:
// 			{
// 				auto child = FormatNode(statement);
// 				indentEnv->AddChild(child);
// 				indentEnv->AddChild(_options.keep_line_after_repeat_statement);
// 				break;
// 			}
// 		case LuaAstNodeType::DoStatement:
// 			{
// 				indentEnv->AddChild(FormatNode(statement));
// 				indentEnv->AddChild(_options.keep_line_after_do_statement);
// 				break;
// 			}
// 		case LuaAstNodeType::ForStatement:
// 			{
// 				indentEnv->AddChild(FormatNode(statement));
// 				indentEnv->AddChild(_options.keep_line_after_for_statement);
// 				break;
// 			}
// 		case LuaAstNodeType::WhileStatement:
// 			{
// 				indentEnv->AddChild(FormatNode(statement));
// 				indentEnv->AddChild(_options.keep_line_after_while_statement);
// 				break;
// 			}
// 		case LuaAstNodeType::IfStatement:
// 			{
// 				indentEnv->AddChild(FormatNode(statement));
// 				indentEnv->AddChild(_options.keep_line_after_if_statement);
// 				break;
// 			}
// 		case LuaAstNodeType::Comment:
// 			{
// 				auto comment = FormatComment(statement);
// 				auto commentStatement = std::make_shared<StatementElement>();
// 				commentStatement->AddChild(comment);
// 				indentEnv->AddChild(commentStatement);
//
// 				indentEnv->Add<KeepLineElement>();
// 				break;
// 			}
// 		case LuaAstNodeType::BreakStatement:
// 		case LuaAstNodeType::ReturnStatement:
// 		case LuaAstNodeType::GotoStatement:
// 		case LuaAstNodeType::ExpressionStatement:
// 			{
// 				auto statEnv = FormatNode(statement);
// 				if (NextMatch(it, LuaAstNodeType::Comment, statements))
// 				{
// 					auto next = NextNode(it, statements);
// 					int currentLine = _parser->GetLine(statement->GetTextRange().EndOffset);
// 					int nextLine = _parser->GetLine(next->GetTextRange().StartOffset);
//
// 					if (currentLine == nextLine)
// 					{
// 						statEnv->Add<SpaceElement>(1);
// 						statEnv->AddChild(FormatComment(next));
// 						++it;
// 					}
// 				}
// 				indentEnv->AddChild(statEnv);
// 				indentEnv->Add<KeepLineElement>();
// 				break;
// 			}
// 		case LuaAstNodeType::LocalFunctionStatement:
// 		case LuaAstNodeType::FunctionStatement:
// 			{
// 				indentEnv->AddChild(FormatNode(statement));
// 				indentEnv->AddChild(_options.keep_line_after_function_define_statement);
// 				break;
// 			}
// 		case LuaAstNodeType::LabelStatement:
// 			{
// 				auto childEnv = FormatNode(statement);
// 				if (_options.label_no_indent)
// 				{
// 					auto noIndent = std::make_shared<NoIndentElement>();
// 					noIndent->AddChild(childEnv);
// 					indentEnv->AddChild(noIndent);
// 				}
// 				else
// 				{
// 					indentEnv->AddChild(childEnv);
// 				}
//
// 				break;
// 			}
// 		default:
// 			{
// 				auto childEnv = FormatNode(statement);
// 				indentEnv->AddChild(childEnv);
// 				indentEnv->Add<KeepLineElement>();
// 			}
// 		}
// 	}
// endLoop:
// 	return indentEnv;
// }

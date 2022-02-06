#include "LuaParser/LuaParser.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include "LuaDefine.h"
#include "LuaParser/LuaTokenTypeDetail.h"
#include "LuaParser/LuaOperatorType.h"
#include "Util/format.h"
#include "LuaParser/LuaParseException.h"
#include "LuaParser/LuaFile.h"

using namespace std::filesystem;

std::shared_ptr<LuaParser> LuaParser::LoadFromFile(std::string_view filename)
{
	std::string realPath(filename);
	std::fstream fin(realPath, std::ios::in | std::ios::binary);

	if (fin.is_open())
	{
		std::stringstream s;
		s << fin.rdbuf();
		auto parser = LoadFromBuffer(s.str());
		parser->SetFilename(realPath);
		return parser;
	}

	return nullptr;
}

std::shared_ptr<LuaParser> LuaParser::LoadFromBuffer(std::string&& buffer)
{
	auto file = std::make_shared<LuaFile>("chunk", std::move(buffer));

	auto tokenParser = std::make_shared<LuaTokenParser>(file);
	tokenParser->Parse();

	return std::make_shared<LuaParser>(tokenParser);
}

void LuaParser::BuildAst()
{
	_chunkAstNode = CreateAstNode(LuaAstNodeType::Chunk);
	try
	{
		Block(_chunkAstNode);
	}
	catch (LuaParseException&)
	{
		// ignore
	}

	auto tokenErrors = _tokenParser->GetErrors();

	for (auto& tokeError : tokenErrors)
	{
		_errors.push_back(tokeError);
	}
}

std::shared_ptr<LuaAstNode> LuaParser::GetAst()
{
	return _chunkAstNode;
}

std::vector<LuaError>& LuaParser::GetErrors()
{
	return _errors;
}

bool LuaParser::HasError() const
{
	return !_errors.empty();
}

std::vector<LuaToken>& LuaParser::GetAllComments()
{
	return _tokenParser->GetComments();
}

int LuaParser::GetLine(int offset) const
{
	return _file->GetLine(offset);
}

int LuaParser::GetColumn(int offset) const
{
	return _file->GetColumn(offset);
}

int LuaParser::GetTotalLine()
{
	return _file->GetTotalLine();
}

std::string_view LuaParser::GetSource()
{
	return _tokenParser->GetSource();
}

bool LuaParser::IsEmptyLine(int line)
{
	return _file->IsEmptyLine(line);
}

void LuaParser::SetFilename(std::string_view filename)
{
#ifdef NOT_SURPPORT_FILE_SYSTEM
	_file->SetFilename(filename);
#else
	std::filesystem::path path(filename);
	_file->SetFilename(path.replace_extension().string());
#endif
}

std::string_view LuaParser::GetFilename()
{
	return _file->GetFilename();
}

std::shared_ptr<LuaFile> LuaParser::GetLuaFile()
{
	return _file;
}

void LuaParser::BuildAstWithComment()
{
	BuildAst();

	auto chunkChildren = _chunkAstNode->GetChildren();
	if (chunkChildren.empty())
	{
		return;
	}

	auto blockNode = chunkChildren.front();

	auto& comments = _tokenParser->GetComments();
	// 将注释注入AST中
	if (!comments.empty())
	{
		for (auto& comment : comments)
		{
			std::shared_ptr<LuaAstNode> commentAst = CreateAstNode(LuaAstNodeType::Comment);
			if (comment.TokenType == TK_SHORT_COMMENT)
			{
				commentAst->AddChild(CreateAstNodeFromToken(LuaAstNodeType::ShortComment, comment));
			}
			else if (comment.TokenType == TK_LONG_COMMENT)
			{
				commentAst->AddChild(CreateAstNodeFromToken(LuaAstNodeType::LongComment, comment));
			}
			else
			{
				commentAst->AddChild(CreateAstNodeFromToken(LuaAstNodeType::ShebangComment, comment));
			}

			blockNode->AddComment(commentAst);
		}
	}

	_tokenParser->ReleaseTokens();
}

LuaParser::LuaParser(std::shared_ptr<LuaTokenParser> tokenParser)
	: _tokenParser(tokenParser),
	  _chunkAstNode(nullptr),
	  _file(tokenParser->GetFile())
{
}

bool LuaParser::BlockFollow(bool withUntil)
{
	switch (_tokenParser->Current().TokenType)
	{
	case TK_ELSE:
	case TK_ELSEIF:
	case TK_END:
	case TK_EOS:
		return true;
	case TK_UNTIL:
		return withUntil;
	default: return false;
	}
}

void LuaParser::StatementList(std::shared_ptr<LuaAstNode> blockNode)
{
	while (!BlockFollow(true))
	{
		Statement(blockNode);
	}
}

void LuaParser::Statement(std::shared_ptr<LuaAstNode> blockNode)
{
	switch (_tokenParser->Current().TokenType)
	{
	case ';':
		{
			auto emptyStatement = CreateAstNodeFromCurrentToken(LuaAstNodeType::EmptyStatement);
			blockNode->AddChild(emptyStatement);
			_tokenParser->Next();
			break;
		}
	case TK_IF:
		{
			IfStatement(blockNode);
			break;
		}
	case TK_WHILE:
		{
			WhileStatement(blockNode);
			break;
		}
	case TK_DO:
		{
			DoStatement(blockNode);
			break;
		}
	case TK_FOR:
		{
			ForStatement(blockNode);
			break;
		}
	case TK_REPEAT:
		{
			RepeatStatement(blockNode);
			break;
		}
	case TK_FUNCTION:
		{
			FunctionStatement(blockNode);
			break;
		}
	case TK_LOCAL:
		{
			if (_tokenParser->LookAhead().TokenType == TK_FUNCTION)
			{
				LocalFunctionStatement(blockNode);
			}
			else
			{
				LocalStatement(blockNode);
			}
			break;
		}
	case TK_DBCOLON:
		{
			LabelStatement(blockNode);
			break;
		}
	case TK_RETURN:
		{
			ReturnStatement(blockNode);
			break;
		}
	case TK_BREAK:
		{
			BreakStatement(blockNode);
			break;
		}
	case TK_GOTO:
		{
			GotoStatement(blockNode);
			break;
		}
	default:
		{
			ExpressionStatement(blockNode);
			break;
		}
	}
}

/* ifstat -> IF cond THEN block {ELSEIF cond THEN block} [ELSE block] END */
void LuaParser::IfStatement(std::shared_ptr<LuaAstNode> blockNode)
{
	auto ifNode = CreateAstNode(LuaAstNodeType::IfStatement);

	TestThenBlock(ifNode);
	while (_tokenParser->Current().TokenType == TK_ELSEIF)
	{
		TestThenBlock(ifNode);
	}
	if (TestNext(TK_ELSE, ifNode))
	{
		Block(ifNode);
	}
	CheckMatch(TK_END, TK_IF, ifNode);

	blockNode->AddChild(ifNode);
}

/* whilestat -> WHILE cond DO block END */
void LuaParser::WhileStatement(std::shared_ptr<LuaAstNode> blockNode)
{
	auto whileStatement = CreateAstNode(LuaAstNodeType::WhileStatement);

	CheckAndNext(TK_WHILE, whileStatement);

	Condition(whileStatement);

	CheckAndNext(TK_DO, whileStatement);

	Block(whileStatement);

	CheckMatch(TK_END, TK_WHILE, whileStatement);

	blockNode->AddChild(whileStatement);
}

void LuaParser::DoStatement(std::shared_ptr<LuaAstNode> blockNode)
{
	auto doStatement = CreateAstNode(LuaAstNodeType::DoStatement);

	CheckAndNext(TK_DO, doStatement);

	Block(doStatement);

	CheckMatch(TK_END, TK_DO, doStatement);

	blockNode->AddChild(doStatement);
}

/* forstat -> FOR (fornum | forlist) END */
void LuaParser::ForStatement(std::shared_ptr<LuaAstNode> blockNode)
{
	// forstatement 只有一个 for 的token 节点 加上 forNumber或者forList 节点
	auto forStatement = CreateAstNode(LuaAstNodeType::ForStatement);

	CheckAndNext(TK_FOR, forStatement);

	Check(TK_NAME);

	switch (_tokenParser->LookAhead().TokenType)
	{
	case '=':
		{
			ForNumber(forStatement);
			break;
		}
	case ',':
	case TK_IN:
		{
			ForList(forStatement);
			break;
		}
	default:
		{
			ThrowLuaError("'=' or 'in' expected", forStatement);
		}
	}

	blockNode->AddChild(forStatement);
}

/* repeatstat -> REPEAT block UNTIL cond */
void LuaParser::RepeatStatement(std::shared_ptr<LuaAstNode> blockNode)
{
	auto repeatStatement = CreateAstNode(LuaAstNodeType::RepeatStatement);

	CheckAndNext(TK_REPEAT, repeatStatement);

	Block(repeatStatement);

	CheckMatch(TK_UNTIL, TK_REPEAT, repeatStatement);

	Condition(repeatStatement);

	blockNode->AddChild(repeatStatement);
}

void LuaParser::FunctionStatement(std::shared_ptr<LuaAstNode> blockNode)
{
	auto functionStatement = CreateAstNode(LuaAstNodeType::FunctionStatement);

	CheckAndNext(TK_FUNCTION, functionStatement);

	FunctionName(functionStatement);

	FunctionBody(functionStatement);

	blockNode->AddChild(functionStatement);
}

void LuaParser::LocalFunctionStatement(std::shared_ptr<LuaAstNode> blockNode)
{
	auto localFunctionStatement = CreateAstNode(LuaAstNodeType::LocalFunctionStatement);

	CheckAndNext(TK_LOCAL, localFunctionStatement);

	CheckAndNext(TK_FUNCTION, localFunctionStatement);

	CheckName(localFunctionStatement);

	FunctionBody(localFunctionStatement);

	blockNode->AddChild(localFunctionStatement);
}

/* stat -> LOCAL ATTRIB NAME {',' ATTRIB NAME} ['=' explist] */
void LuaParser::LocalStatement(std::shared_ptr<LuaAstNode> blockNode)
{
	auto localStatement = CreateAstNode(LuaAstNodeType::LocalStatement);

	CheckAndNext(TK_LOCAL, localStatement);

	auto nameDefList = CreateAstNode(LuaAstNodeType::NameDefList);

	do
	{
		CheckName(nameDefList);
		GetLocalAttribute(nameDefList);
	}
	while (TestNext(',', nameDefList, LuaAstNodeType::GeneralOperator));

	localStatement->AddChild(nameDefList);

	if (TestNext('=', localStatement, LuaAstNodeType::GeneralOperator))
	{
		ExpressionList(localStatement);
	}
	// 如果有一个分号则加入到localstatement
	TestNext(';', localStatement, LuaAstNodeType::GeneralOperator);

	blockNode->AddChild(localStatement);
}

void LuaParser::LabelStatement(std::shared_ptr<LuaAstNode> blockNode)
{
	auto labelStatement = CreateAstNode(LuaAstNodeType::LabelStatement);

	CheckAndNext(TK_DBCOLON, labelStatement);

	CheckName(labelStatement);

	CheckAndNext(TK_DBCOLON, labelStatement);

	//认为label是单独的语句
	blockNode->AddChild(labelStatement);
}

void LuaParser::ReturnStatement(std::shared_ptr<LuaAstNode> blockNode)
{
	auto returnStatement = CreateAstNode(LuaAstNodeType::ReturnStatement);

	CheckAndNext(TK_RETURN, returnStatement);

	if (!(BlockFollow(true) || _tokenParser->Current().TokenType == ';'))
	{
		ExpressionList(returnStatement);
	}

	TestNext(';', returnStatement, LuaAstNodeType::GeneralOperator);

	blockNode->AddChild(returnStatement);
}


void LuaParser::BreakStatement(std::shared_ptr<LuaAstNode> blockNode)
{
	auto breakStatement = CreateAstNode(LuaAstNodeType::BreakStatement);

	CheckAndNext(TK_BREAK, breakStatement);
	blockNode->AddChild(breakStatement);
}

void LuaParser::GotoStatement(std::shared_ptr<LuaAstNode> blockNode)
{
	auto gotoStatement = CreateAstNode(LuaAstNodeType::GotoStatement);

	CheckAndNext(TK_GOTO, gotoStatement);

	CheckName(gotoStatement);

	blockNode->AddChild(gotoStatement);
}

/* stat -> func | assignment */
/*
 * 以上是lua定义
 * 以下是修改定义
 * exprstat -> func
 * assignment -> exprList '=' exprList
 */
void LuaParser::ExpressionStatement(std::shared_ptr<LuaAstNode> blockNode)
{
	auto expression = CreateAstNode(LuaAstNodeType::Expression);
	SuffixedExpression(expression);
	if (_tokenParser->Current().TokenType == '=' || _tokenParser->Current().TokenType == ',')
	{
		auto expressionList = CreateAstNode(LuaAstNodeType::ExpressionList);

		expressionList->AddChild(expression);

		auto assignStatementNode = CreateAstNode(LuaAstNodeType::AssignStatement);

		AssignStatement(expressionList, assignStatementNode);

		// 如果发现一个分号，会认为分号为该语句的结尾
		TestNext(';', assignStatementNode, LuaAstNodeType::GeneralOperator);

		blockNode->AddChild(assignStatementNode);
	}
	else
	{
		auto expressionStatement = CreateAstNode(LuaAstNodeType::ExpressionStatement);

		expressionStatement->AddChild(expression);

		TestNext(';', expressionStatement, LuaAstNodeType::GeneralOperator);

		blockNode->AddChild(expressionStatement);
	}
}

/*
** Parse and compile a multiple assignment. The first "variable"
** (a 'suffixedexp') was already read by the caller.
**
** assignment -> suffixedexp restassign
** restassign -> ',' suffixedexp restassign | '=' explist
*/
void LuaParser::AssignStatement(std::shared_ptr<LuaAstNode> expressionListNode,
                                std::shared_ptr<LuaAstNode> assignStatementNode)
{
	if (TestNext(',', expressionListNode, LuaAstNodeType::GeneralOperator))
	{
		auto expression = CreateAstNode(LuaAstNodeType::Expression);
		SuffixedExpression(expression);

		expressionListNode->AddChild(expression);

		AssignStatement(expressionListNode, assignStatementNode);
	}
	else
	{
		assignStatementNode->AddChild(expressionListNode);
		CheckAndNext('=', assignStatementNode, LuaAstNodeType::GeneralOperator);
		ExpressionList(assignStatementNode);
	}
}


void LuaParser::Condition(std::shared_ptr<LuaAstNode> parent)
{
	Expression(parent);
}


/* test_then_block -> [IF | ELSEIF] cond THEN block */
void LuaParser::TestThenBlock(std::shared_ptr<LuaAstNode> ifNode)
{
	auto ifOrElseifKeyNode = CreateAstNodeFromToken(LuaAstNodeType::KeyWord, _tokenParser->Current());
	ifNode->AddChild(ifOrElseifKeyNode);

	_tokenParser->Next(); /*skip if or elseif*/
	Expression(ifNode);
	CheckAndNext(TK_THEN, ifNode);
	Block(ifNode);
}


void LuaParser::Block(std::shared_ptr<LuaAstNode> parent)
{
	auto blockNode = CreateAstNode(LuaAstNodeType::Block);

	StatementList(blockNode);

	parent->AddChild(blockNode);
}

void LuaParser::CheckMatch(LuaTokenType what, LuaTokenType who, std::shared_ptr<LuaAstNode> parent,
                           LuaAstNodeType addType)
{
	if (!TestNext(what, parent, addType))
	{
		auto range = parent->GetTextRange();
		ThrowMatchError(format("token {} expected ,(to close {} at", what, who),
		                TextRange(range.EndOffset, range.EndOffset));
	}
}

/* explist -> expr { ',' expr } */
void LuaParser::ExpressionList(std::shared_ptr<LuaAstNode> parent)
{
	auto expressionListNode = CreateAstNode(LuaAstNodeType::ExpressionList);
	Expression(expressionListNode);
	while (TestNext(',', expressionListNode, LuaAstNodeType::GeneralOperator))
	{
		Expression(expressionListNode);
	}

	parent->AddChild(expressionListNode);
}

void LuaParser::Expression(std::shared_ptr<LuaAstNode> parent)
{
	// 所有具体表达式树都在表达式节点下
	// 例如 单一元表达式 作为 表达式节点的一个child
	// 复杂表达式的顶层也作为表达式节点的child
	auto expressionNode = CreateAstNode(LuaAstNodeType::Expression);
	Subexpression(expressionNode, 0);

	parent->AddChild(expressionNode);
}

/*
* subexpr -> (simpleexp | unop subexpr) { binop subexpr }
*/
void LuaParser::Subexpression(std::shared_ptr<LuaAstNode> expressionNode, int limit)
{
	// 假设都是二元表达式
	auto binaryExpression = CreateAstNode(LuaAstNodeType::BinaryExpression);

	UnOpr uop = GetUnaryOperator(_tokenParser->Current().TokenType);
	if (uop != UnOpr::OPR_NOUNOPR) /* prefix (unary) operator? */
	{
		auto unaryExpression = CreateAstNode(LuaAstNodeType::UnaryExpression);

		auto unaryTokenNode = CreateAstNodeFromToken(LuaAstNodeType::UnaryOperator, _tokenParser->Current());
		unaryExpression->AddChild(unaryTokenNode);

		_tokenParser->Next();
		Subexpression(unaryExpression, UNARY_PRIORITY);

		binaryExpression->AddChild(unaryExpression);
	}
	else
	{
		SimpleExpression(binaryExpression);
	}


	bool isBinary = false;

	auto op = GetBinaryOperator(_tokenParser->Current().TokenType);
	/* expand while operators have priorities higher than 'limit' */
	while (op != BinOpr::OPR_NOBINOPR && priority[static_cast<int>(op)].left > limit)
	{
		if (isBinary)
		{
			auto topBinaryExpression = CreateAstNode(LuaAstNodeType::BinaryExpression);
			topBinaryExpression->AddChild(binaryExpression);
			binaryExpression = topBinaryExpression;
		}
		else
		{
			isBinary = true;
		}

		auto binaryOperator = CreateAstNodeFromToken(LuaAstNodeType::BinaryOperator, _tokenParser->Current());
		binaryExpression->AddChild(binaryOperator);

		_tokenParser->Next();
		Subexpression(binaryExpression, priority[static_cast<int>(op)].right);

		// next op
		op = GetBinaryOperator(_tokenParser->Current().TokenType);
	}


	if (isBinary)
	{
		expressionNode->AddChild(binaryExpression);
	}
	else
	{
		auto children = binaryExpression->GetChildren();
		for (auto child : children)
		{
			expressionNode->AddChild(child);
		}
	}
}

/* simpleexp -> FLT | INT | STRING | NIL | TRUE | FALSE | ... |
				constructor | FUNCTION body | suffixedexp */
void LuaParser::SimpleExpression(std::shared_ptr<LuaAstNode> expressionNode)
{
	switch (_tokenParser->Current().TokenType)
	{
	case TK_FLT:
	case TK_INT:
	case TK_STRING:
	case TK_NIL:
	case TK_TRUE:
	case TK_FALSE:
	case TK_DOTS:
		{
			auto tokenNode = CreateAstNodeFromToken(LuaAstNodeType::LiteralExpression, _tokenParser->Current());
			expressionNode->AddChild(tokenNode);
			_tokenParser->Next();
			break;
		}
	case '{':
		{
			TableConstructor(expressionNode);
			break;
		}
	case TK_FUNCTION:
		{
			auto closureNode = CreateAstNode(LuaAstNodeType::ClosureExpression);
			auto functionKeyWordNode = CreateAstNodeFromToken(LuaAstNodeType::KeyWord, _tokenParser->Current());
			closureNode->AddChild(functionKeyWordNode);

			_tokenParser->Next();
			FunctionBody(closureNode);

			expressionNode->AddChild(closureNode);
			break;
		}
	default:
		{
			SuffixedExpression(expressionNode);
			return;
		}
	}
}

/* constructor -> '{' [ field { sep field } [sep] ] '}'
   sep -> ',' | ';'
*/
void LuaParser::TableConstructor(std::shared_ptr<LuaAstNode> expressionNode)
{
	auto tableExpression = CreateAstNode(LuaAstNodeType::TableExpression);
	CheckAndNext('{', tableExpression, LuaAstNodeType::GeneralOperator);

	do
	{
		if (_tokenParser->Current().TokenType == '}')
		{
			break;
		}
		Field(tableExpression);
	}
	while (TestNext(',', tableExpression, LuaAstNodeType::TableFieldSep)
		|| TestNext(';', tableExpression, LuaAstNodeType::TableFieldSep));

	CheckMatch('}', '{', tableExpression, LuaAstNodeType::GeneralOperator);

	expressionNode->AddChild(tableExpression);
}

/* field -> listfield | recfield */
void LuaParser::Field(std::shared_ptr<LuaAstNode> tableExpressionNode)
{
	auto tableFieldNode = CreateAstNode(LuaAstNodeType::TableField);
	switch (_tokenParser->Current().TokenType)
	{
	case TK_NAME:
		{
			if (_tokenParser->LookAhead().TokenType != '=')
			{
				ListField(tableFieldNode);
			}
			else
			{
				RectField(tableFieldNode);
			}
			break;
		}
	case '[':
		{
			RectField(tableFieldNode);
			break;
		}
	default:
		{
			ListField(tableFieldNode);
			break;
		}
	}

	tableExpressionNode->AddChild(tableFieldNode);
}

void LuaParser::ListField(std::shared_ptr<LuaAstNode> tableFieldNode)
{
	Expression(tableFieldNode);
}

/* recfield -> (NAME | '['exp']') = exp */
void LuaParser::RectField(std::shared_ptr<LuaAstNode> tableFieldNode)
{
	if (_tokenParser->Current().TokenType == TK_NAME)
	{
		CodeName(tableFieldNode);
	}
	else
	{
		YIndex(tableFieldNode);
	}

	CheckAndNext('=', tableFieldNode, LuaAstNodeType::GeneralOperator);

	Expression(tableFieldNode);
}

/* body ->  '(' parlist ')' block END */
void LuaParser::FunctionBody(std::shared_ptr<LuaAstNode> closureExpression)
{
	auto functionBodyNode = CreateAstNode(LuaAstNodeType::FunctionBody);

	ParamList(functionBodyNode);

	Block(functionBodyNode);

	CheckMatch(TK_END, TK_FUNCTION, functionBodyNode);

	closureExpression->AddChild(functionBodyNode);
}

void LuaParser::ParamList(std::shared_ptr<LuaAstNode> functionBodyNode)
{
	auto paramList = CreateAstNode(LuaAstNodeType::ParamList);

	CheckAndNext('(', paramList, LuaAstNodeType::GeneralOperator);

	bool isVararg = false;
	if (_tokenParser->Current().TokenType != ')')
	{
		do
		{
			switch (_tokenParser->Current().TokenType)
			{
			case TK_NAME:
				{
					auto paramNode = CreateAstNodeFromToken(LuaAstNodeType::Param, _tokenParser->Current());
					paramList->AddChild(paramNode);
					_tokenParser->Next();
					break;
				}
			case TK_DOTS:
				{
					isVararg = true;
					auto paramNode = CreateAstNodeFromToken(LuaAstNodeType::Param, _tokenParser->Current());
					paramList->AddChild(paramNode);
					_tokenParser->Next();
					break;
				}
			default:
				{
					ThrowLuaError("<name> or '...' expected", paramList);
				}
			}
		}
		while (!isVararg && TestNext(',', paramList, LuaAstNodeType::GeneralOperator));
	}

	CheckAndNext(')', paramList, LuaAstNodeType::GeneralOperator);

	functionBodyNode->AddChild(paramList);
}


/* suffixedexp ->
	 primaryexp { '.' NAME | '[' exp ']' | ':' NAME funcargs | funcargs } */
/* 以上是lua原始定义
 * 下面是修改定义
 * suffixedexp -> indexexpr | callexpr | primaryexp
 * indexexpr -> primaryexp { '.' NAME | '[' exp ']' | ':' NAME  }
 * callexpr -> primaryexp funcargs | indexexpr funcargs
 */
void LuaParser::SuffixedExpression(std::shared_ptr<LuaAstNode> expressionNode)
{
	auto subExpression = CreateAstNode(LuaAstNodeType::PrimaryExpression);

	PrimaryExpression(subExpression);
	for (;;)
	{
		switch (_tokenParser->Current().TokenType)
		{
		case '.':
			{
				FieldSel(subExpression);
				break;
			}
		case '[':
			{
				auto indexExpr = CreateAstNode(LuaAstNodeType::IndexExpression);
				indexExpr->AddChild(subExpression);
				YIndex(indexExpr);
				subExpression = indexExpr;
				break;
			}
		case ':':
			{
				FieldSel(subExpression);
				FunctionCallArgs(subExpression);
				break;
			}
		case '(':
		case TK_STRING:
		case '{':
			{
				FunctionCallArgs(subExpression);
				break;
			}
		default:
			expressionNode->AddChild(subExpression);
			return;
		}
	}
}

void LuaParser::FunctionCallArgs(std::shared_ptr<LuaAstNode>& expressionNode)
{
	auto callExpression = CreateAstNode(LuaAstNodeType::CallExpression);

	callExpression->AddChild(expressionNode);

	auto callArgsNode = CreateAstNode(LuaAstNodeType::CallArgList);
	switch (_tokenParser->Current().TokenType)
	{
	case '(':
		{
			auto tokenNode = CreateAstNodeFromCurrentToken(LuaAstNodeType::GeneralOperator);
			callArgsNode->AddChild(tokenNode);
			_tokenParser->Next();
			if (_tokenParser->Current().TokenType != ')')
			{
				ExpressionList(callArgsNode);
			}

			CheckMatch(')', '(', callArgsNode, LuaAstNodeType::GeneralOperator);
			break;
		}
	case '{':
		{
			TableConstructor(callArgsNode);
			break;
		}
	case TK_STRING:
		{
			auto stringNode = CreateAstNodeFromCurrentToken(LuaAstNodeType::LiteralExpression);
			callArgsNode->AddChild(stringNode);
			_tokenParser->Next();
			break;
		}
	default:
		{
			ThrowLuaError("function arguments expected", callArgsNode);
		}
	}

	callExpression->AddChild(callArgsNode);

	expressionNode = callExpression;
}

/* fieldsel -> ['.' | ':'] NAME */
void LuaParser::FieldSel(std::shared_ptr<LuaAstNode>& expressionNode)
{
	auto indexExpression = CreateAstNode(LuaAstNodeType::IndexExpression);

	indexExpression->AddChild(expressionNode);

	auto tokenNode = CreateAstNodeFromCurrentToken(LuaAstNodeType::IndexOperator);
	indexExpression->AddChild(tokenNode);

	_tokenParser->Next();
	CodeName(indexExpression);

	expressionNode = indexExpression;
}

/* index -> '[' expr ']' */
void LuaParser::YIndex(std::shared_ptr<LuaAstNode> expressionNode)
{
	CheckAndNext('[', expressionNode, LuaAstNodeType::IndexOperator);

	Expression(expressionNode);
	CheckAndNext(']', expressionNode, LuaAstNodeType::IndexOperator);
}

void LuaParser::FunctionName(std::shared_ptr<LuaAstNode> functionNode)
{
	auto expression = CreateAstNode(LuaAstNodeType::PrimaryExpression);

	CheckName(expression);

	while (_tokenParser->Current().TokenType == '.')
	{
		FieldSel(expression);
	}
	if (_tokenParser->Current().TokenType == ':')
	{
		FieldSel(expression);
	}

	auto nameExpr = CreateAstNode(LuaAstNodeType::NameExpression);

	nameExpr->AddChild(expression);

	functionNode->AddChild(nameExpr);
}

std::string_view LuaParser::CheckName(std::shared_ptr<LuaAstNode> parent)
{
	Check(TK_NAME);

	auto identify = CreateAstNodeFromCurrentToken(LuaAstNodeType::Identify);

	parent->AddChild(identify);

	_tokenParser->Next();

	return identify->GetText();
}

/* ATTRIB -> ['<' Name '>'] */
LuaAttribute LuaParser::GetLocalAttribute(std::shared_ptr<LuaAstNode> nameDefList)
{
	auto attribute = CreateAstNode(LuaAstNodeType::Attribute);

	if (TestNext('<', attribute, LuaAstNodeType::GeneralOperator))
	{
		auto attributeName = CheckName(attribute);
		CheckAndNext('>', attribute, LuaAstNodeType::GeneralOperator);

		nameDefList->AddChild(attribute);

		if (attributeName == "const")
		{
			return LuaAttribute::Const;
		}
		else if (attributeName == "close")
		{
			return LuaAttribute::Close;
		}

		ThrowMatchError(format("unknown attribute {}", attributeName), attribute->GetTextRange());
	}

	return LuaAttribute::NoAttribute;
}

void LuaParser::Check(LuaTokenType c)
{
	if (_tokenParser->Current().TokenType != c)
	{
		ThrowMatchError(format("{} expected", c), _tokenParser->Current().Range);
	}
}


/* primaryexp -> NAME | '(' expr ')' */
void LuaParser::PrimaryExpression(std::shared_ptr<LuaAstNode> primaryExpression)
{
	switch (_tokenParser->Current().TokenType)
	{
	case '(':
		{
			auto leftBreaketToken = CreateAstNodeFromCurrentToken(LuaAstNodeType::GeneralOperator);
			primaryExpression->AddChild(leftBreaketToken);
			_tokenParser->Next();

			Expression(primaryExpression);
			CheckMatch(')', '(', primaryExpression);
			return;
		}
	case TK_NAME:
		{
			auto identify = CreateAstNodeFromCurrentToken(LuaAstNodeType::NameIdentify);
			primaryExpression->AddChild(identify);
			_tokenParser->Next();
			// CodeName(primaryExpression);
			return;
		}
	default:
		ThrowLuaError("unexpected symbol", primaryExpression);
	}
}

UnOpr LuaParser::GetUnaryOperator(LuaTokenType op)
{
	switch (op)
	{
	case TK_NOT:
		{
			return UnOpr::OPR_NOT;
		}
	case '-':
		{
			return UnOpr::OPR_MINUS;
		}
	case '~':
		{
			return UnOpr::OPR_BNOT;
		}
	case '#':
		{
			return UnOpr::OPR_LEN;
		}
	default:
		{
			return UnOpr::OPR_NOUNOPR;
		}
	}
}

BinOpr LuaParser::GetBinaryOperator(LuaTokenType op)
{
	switch (op)
	{
	case '+': return BinOpr::OPR_ADD;
	case '-': return BinOpr::OPR_SUB;
	case '*': return BinOpr::OPR_MUL;
	case '%': return BinOpr::OPR_MOD;
	case '^': return BinOpr::OPR_POW;
	case '/': return BinOpr::OPR_DIV;
	case TK_IDIV: return BinOpr::OPR_IDIV;
	case '&': return BinOpr::OPR_BAND;
	case '|': return BinOpr::OPR_BOR;
	case '~': return BinOpr::OPR_BXOR;
	case TK_SHL: return BinOpr::OPR_SHL;
	case TK_SHR: return BinOpr::OPR_SHR;
	case TK_CONCAT: return BinOpr::OPR_CONCAT;
	case TK_NE: return BinOpr::OPR_NE;
	case TK_EQ: return BinOpr::OPR_EQ;
	case '<': return BinOpr::OPR_LT;
	case TK_LE: return BinOpr::OPR_LE;
	case '>': return BinOpr::OPR_GT;
	case TK_GE: return BinOpr::OPR_GE;
	case TK_AND: return BinOpr::OPR_AND;
	case TK_OR: return BinOpr::OPR_OR;
	default: return BinOpr::OPR_NOBINOPR;
	}
}

void LuaParser::CheckAndNext(LuaTokenType c, std::shared_ptr<LuaAstNode> parent, LuaAstNodeType addType)
{
	if (_tokenParser->Current().TokenType != c)
	{
		ThrowLuaError(format("token type {} expected", c), parent);
		return;
	}

	parent->AddChild(CreateAstNodeFromCurrentToken(addType));

	_tokenParser->Next();
}

bool LuaParser::TestNext(LuaTokenType c, std::shared_ptr<LuaAstNode> parent, LuaAstNodeType addType)
{
	if (_tokenParser->Current().TokenType == c)
	{
		parent->AddChild(CreateAstNodeFromToken(addType, _tokenParser->Current()));
		_tokenParser->Next();
		return true;
	}
	return false;
}

void LuaParser::CodeName(std::shared_ptr<LuaAstNode> parent)
{
	if (_tokenParser->Current().TokenType == TK_NAME)
	{
		auto identify = CreateAstNodeFromToken(LuaAstNodeType::Identify, _tokenParser->Current());
		parent->AddChild(identify);
		_tokenParser->Next();
	}
}


std::shared_ptr<LuaAstNode> LuaParser::CreateAstNode(LuaAstNodeType type)
{
	return std::make_shared<LuaAstNode>(type, std::string_view(GetSource().data(), 0), TextRange());
}

std::shared_ptr<LuaAstNode> LuaParser::CreateAstNodeFromToken(LuaAstNodeType type, LuaToken& token)
{
	return std::make_shared<LuaAstNode>(type, token);
}

std::shared_ptr<LuaAstNode> LuaParser::CreateAstNodeFromCurrentToken(LuaAstNodeType type)
{
	return CreateAstNodeFromToken(type, _tokenParser->Current());
}

void LuaParser::ThrowLuaError(std::string_view message, std::shared_ptr<LuaAstNode> parent)
{
	if (_tokenParser->Current().TokenType != TK_EOS)
	{
		auto tokenNode = CreateAstNodeFromCurrentToken(LuaAstNodeType::Error);
		parent->AddChild(tokenNode);
		_tokenParser->Next();
		_errors.emplace_back(message, tokenNode->GetTextRange());
	}
	else
	{
		auto offset = _tokenParser->LastValidOffset();
		if (offset != 0)
		{
			_errors.emplace_back(message, TextRange(offset, offset));
		}
	}
	throw LuaParseException(message);
}

void LuaParser::ThrowMatchError(std::string message, TextRange range)
{
	_errors.emplace_back(message, range);
	throw LuaParseException(message);
}

void LuaParser::ForNumber(std::shared_ptr<LuaAstNode> forStatement)
{
	auto forNumberNode = CreateAstNode(LuaAstNodeType::ForNumber);

	CheckName(forNumberNode);

	CheckAndNext('=', forNumberNode, LuaAstNodeType::GeneralOperator);

	Expression(forNumberNode);

	CheckAndNext(',', forNumberNode, LuaAstNodeType::GeneralOperator);

	Expression(forNumberNode);

	if (TestNext(',', forNumberNode, LuaAstNodeType::GeneralOperator)) // optional step
	{
		Expression(forNumberNode);
	}

	ForBody(forNumberNode);

	forStatement->AddChild(forNumberNode);
}

/* forlist -> NAME {,NAME} IN explist forbody */
void LuaParser::ForList(std::shared_ptr<LuaAstNode> forStatement)
{
	auto forListNode = CreateAstNode(LuaAstNodeType::ForList);

	auto nameDefList = CreateAstNode(LuaAstNodeType::NameDefList);

	CheckName(nameDefList);

	while (TestNext(',', nameDefList, LuaAstNodeType::GeneralOperator))
	{
		CheckName(nameDefList);
	}

	forListNode->AddChild(nameDefList);

	CheckAndNext(TK_IN, forListNode);

	ExpressionList(forListNode);

	ForBody(forListNode);

	forStatement->AddChild(forListNode);
}

void LuaParser::ForBody(std::shared_ptr<LuaAstNode> forNode)
{
	auto forBodyNode = CreateAstNode(LuaAstNodeType::ForBody);

	CheckAndNext(TK_DO, forBodyNode);

	Block(forBodyNode);

	CheckMatch(TK_END, TK_FOR, forBodyNode);

	forNode->AddChild(forBodyNode);
}

#include "LuaParser/LuaParser.h"

#include "LuaDefine.h"
#include "LuaTokenTypeDetail.h"
#include "LuaParser/LuaOperatorType.h"
#include "Util/format.h"

bool LuaParser::Parse()
{
	_chunkAstNode = createAstNode(LuaAstNodeType::Chunk);

	block(_chunkAstNode);

	return true;
}

std::shared_ptr<LuaAstNode> LuaParser::GetAst()
{
	return _chunkAstNode;
}

LuaParser::LuaParser(std::shared_ptr<LuaTokenParser> tokenParser)
	: _tokenParser(tokenParser),
	  _chunkAstNode(nullptr)
{
}

bool LuaParser::blockFollow(bool withUntil)
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

void LuaParser::statementList(std::shared_ptr<LuaAstNode> blockNode)
{
	while (blockFollow(true))
	{
		statement(blockNode);
	}
}

void LuaParser::statement(std::shared_ptr<LuaAstNode> blockNode)
{
	switch (_tokenParser->Current().TokenType)
	{
	case ';':
		{
			_tokenParser->Next();
			break;
		}
	case TK_IF:
		{
			ifStatement(blockNode);
			break;
		}
	}
}

/* ifstat -> IF cond THEN block {ELSEIF cond THEN block} [ELSE block] END */
void LuaParser::ifStatement(std::shared_ptr<LuaAstNode> blockNode)
{
	auto ifNode = createAstNode(LuaAstNodeType::IfStatement);

	testThenBlock(ifNode);
	while (_tokenParser->Current().TokenType == TK_ELSEIF)
	{
		testThenBlock(ifNode);
	}
	if (testNext(TK_ELSE, ifNode))
	{
		block(ifNode);
	}
	checkMatch(TK_END, TK_IF, ifNode);

	blockNode->AddChild(ifNode);
}

/* test_then_block -> [IF | ELSEIF] cond THEN block */
void LuaParser::testThenBlock(std::shared_ptr<LuaAstNode> ifNode)
{
	auto ifOrElseifKeyNode = createAstNodeFromToken(LuaAstNodeType::KeyWord, _tokenParser->Current());
	ifNode->AddChild(ifOrElseifKeyNode);

	_tokenParser->Next(); /*skip if or elseif*/
	expression(ifNode);
}


void LuaParser::block(std::shared_ptr<LuaAstNode> parent)
{
	auto blockNode = createAstNode(LuaAstNodeType::Block);

	statementList(blockNode);

	parent->AddChild(blockNode);
}

void LuaParser::checkMatch(LuaTokenType what, LuaTokenType who, std::shared_ptr<LuaAstNode> parent)
{
	if (!testNext(what, parent))
	{
		throw LuaParserException(format("token {} expected ,(to close {} at", what, who));
	}
}

void LuaParser::expression(std::shared_ptr<LuaAstNode> parent)
{
	// 所有具体表达式树都在表达式节点下
	// 例如 单一元表达式 作为 表达式节点的一个child
	// 复杂表达式的顶层也作为表达式节点的child
	auto expressionNode = createAstNode(LuaAstNodeType::Expression);
	subexpression(parent, 0);

	parent->AddChild(expressionNode);
}

/*
* subexpr -> (simpleexp | unop subexpr) { binop subexpr }
*/
void LuaParser::subexpression(std::shared_ptr<LuaAstNode> expressionNode, int limit)
{
	// 假设都是二元表达式
	auto binaryExpression = createAstNode(LuaAstNodeType::BinaryExpression);

	UnOpr uop = getUnaryOperator(_tokenParser->Current().TokenType);
	if (uop != UnOpr::OPR_NOUNOPR) /* prefix (unary) operator? */
	{
		auto unaryExpression = createAstNode(LuaAstNodeType::UnaryExpression);

		auto unaryTokenNode = createAstNodeFromToken(LuaAstNodeType::UnaryOperator, _tokenParser->Current());
		unaryExpression->AddChild(unaryTokenNode);

		_tokenParser->Next();
		subexpression(unaryExpression, UNARY_PRIORITY);

		binaryExpression->AddChild(unaryExpression);
	}
	else
	{
		simpleExpression(binaryExpression);
	}


	bool isBinary = false;

	auto op = getBinaryOperator(_tokenParser->Current().TokenType);
	/* expand while operators have priorities higher than 'limit' */
	while (op != BinOpr::OPR_NOBINOPR && priority[static_cast<int>(op)].left > limit)
	{
		if (isBinary)
		{
			auto topBinaryExpression = createAstNode(LuaAstNodeType::BinaryExpression);
			topBinaryExpression->AddChild(binaryExpression);
			binaryExpression = topBinaryExpression;
		}
		else
		{
			isBinary = true;
		}

		auto binaryOperator = createAstNodeFromToken(LuaAstNodeType::BinaryOperator, _tokenParser->Current());
		binaryExpression->AddChild(binaryOperator);

		_tokenParser->Next();
		subexpression(binaryExpression, priority[static_cast<int>(op)].right);

		// next op
		op = getBinaryOperator(_tokenParser->Current().TokenType);
	}


	if (isBinary)
	{
		expressionNode->AddChild(binaryExpression);
	}
	else
	{
		auto& children = binaryExpression->GetChildren();
		for (auto child : children)
		{
			expressionNode->AddChild(child);
		}
	}
}

/* simpleexp -> FLT | INT | STRING | NIL | TRUE | FALSE | ... |
				constructor | FUNCTION body | suffixedexp */
void LuaParser::simpleExpression(std::shared_ptr<LuaAstNode> expressionNode)
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
			auto tokenNode = createAstNodeFromToken(LuaAstNodeType::LiteralExpression, _tokenParser->Current());
			expressionNode->AddChild(tokenNode);
			break;
		}
	case '{':
		{
			tableConstructor(expressionNode);
			break;
		}
	case TK_FUNCTION:
		{
			auto closureNode = createAstNode(LuaAstNodeType::ClosureExpression);
			auto functionKeyWordNode = createAstNodeFromToken(LuaAstNodeType::KeyWord, _tokenParser->Current());
			closureNode->AddChild(functionKeyWordNode);

			_tokenParser->Next();
			functionBody(closureNode);

			expressionNode->AddChild(closureNode);
			break;
		}
	default:
		{
			suffixedExpression(expressionNode);
			return;
		}
	}
	_tokenParser->Next();
}

/* constructor -> '{' [ field { sep field } [sep] ] '}'
   sep -> ',' | ';'
*/
void LuaParser::tableConstructor(std::shared_ptr<LuaAstNode> expressionNode)
{
	auto tableExpression = createAstNode(LuaAstNodeType::TableExpression);
	checkNext('{', tableExpression);

	do
	{
		if (_tokenParser->Current().TokenType == '}')
		{
			break;
		}
		field(tableExpression);
	}
	while (testNext(',', tableExpression) || testNext(';', tableExpression));

	checkMatch('}', '{', tableExpression);

	expressionNode->AddChild(tableExpression);
}

/* field -> listfield | recfield */
void LuaParser::field(std::shared_ptr<LuaAstNode> tableExpressionNode)
{
	auto tableFieldNode = createAstNode(LuaAstNodeType::TableField);
	switch (_tokenParser->Current().TokenType)
	{
	case TK_NAME:
		{
			if (_tokenParser->LookAhead().TokenType != '=')
			{
				listField(tableFieldNode);
			}
			else
			{
				rectField(tableFieldNode);
			}
			break;
		}
	case '[':
		{
			rectField(tableFieldNode);
			break;
		}
	default:
		{
			listField(tableFieldNode);
			break;
		}
	}

	tableExpressionNode->AddChild(tableFieldNode);
}

void LuaParser::listField(std::shared_ptr<LuaAstNode> tableExpressionNode)
{
}

void LuaParser::rectField(std::shared_ptr<LuaAstNode> tableExpressionNode)
{
}

/* body ->  '(' parlist ')' block END */
void LuaParser::functionBody(std::shared_ptr<LuaAstNode> closureExpression)
{
	auto functionBodyNode = createAstNode(LuaAstNodeType::FunctionBody);

	paramList(functionBodyNode);

	block(functionBodyNode);

	checkMatch(TK_END, TK_FUNCTION, functionBodyNode);

	closureExpression->AddChild(functionBodyNode);
}

void LuaParser::paramList(std::shared_ptr<LuaAstNode> functionBodyNode)
{
	auto paramList = createAstNode(LuaAstNodeType::ParamList);

	checkNext('(', paramList);

	bool isVararg = false;
	if (_tokenParser->Current().TokenType != ')')
	{
		do
		{
			switch (_tokenParser->Current().TokenType)
			{
			case TK_NAME:
				{
					auto paramNode = createAstNodeFromToken(LuaAstNodeType::Param, _tokenParser->Current());
					paramList->AddChild(paramNode);
					_tokenParser->Next();
					break;
				}
			case TK_DOTS:
				{
					isVararg = true;
					auto paramNode = createAstNodeFromToken(LuaAstNodeType::Param, _tokenParser->Current());
					paramList->AddChild(paramNode);
					_tokenParser->Next();
					break;
				}
			default:
				{
					throw LuaParserException("<name> or '...' expected");
				}
			}
		}
		while (!isVararg && testNext(',', paramList));
	}

	checkNext(')', paramList);

	functionBodyNode->AddChild(paramList);
}

/* suffixedexp ->
	 primaryexp { '.' NAME | '[' exp ']' | ':' NAME funcargs | funcargs } */
void LuaParser::suffixedExpression(std::shared_ptr<LuaAstNode> expressionNode)
{
	auto subExpression = createAstNode(LuaAstNodeType::Expression);

	primaryExpression(subExpression);
	for (;;)
	{
		switch (_tokenParser->Current().TokenType)
		{
		case '.':
			{
				fieldSel(subExpression);
				break;
			}
		case '[':
			{
				auto tokenNode = createAstNodeFromToken(LuaAstNodeType::IndexOperator, _tokenParser->Current());
				subExpression->AddChild(tokenNode);
				yIndex(subExpression);
			}
		case ':':
			{
				fieldSel(subExpression);
				functionArgs();
				break;
			}
		case '(':
		case TK_STRING:
		case '{':
			{
				functionArgs();
				break;
			}
		default:
			return;
		}
	}
}

/* fieldsel -> ['.' | ':'] NAME */
void LuaParser::fieldSel(std::shared_ptr<LuaAstNode> expressionNode)
{
	auto tokenNode = createAstNodeFromToken(LuaAstNodeType::IndexOperator, _tokenParser->Current());
	expressionNode->AddChild(tokenNode);

	_tokenParser->Next();
}

/* index -> '[' expr ']' */
void LuaParser::yIndex(std::shared_ptr<LuaAstNode> expressionNode)
{
	_tokenParser->Next();
	expression(expressionNode);
	checkNext(']', expressionNode);
}


/* primaryexp -> NAME | '(' expr ')' */
void LuaParser::primaryExpression(std::shared_ptr<LuaAstNode> expressionNode)
{
	switch (_tokenParser->Current().TokenType)
	{
	case '(':
		{
			auto leftBreaketToken = createAstNodeFromToken(LuaAstNodeType::KeyWord, _tokenParser->Current());
			expressionNode->AddChild(leftBreaketToken);

			expression(expressionNode);
			checkMatch(')', '(', expressionNode);
			return;
		}
	case TK_NAME:
		{
			auto identify = createAstNodeFromToken(LuaAstNodeType::Identify, _tokenParser->Current());
			expressionNode->AddChild(identify);
			return;
		}
	default:
		throw LuaParserException("unexpected symbol");
	}
}

UnOpr LuaParser::getUnaryOperator(LuaTokenType op)
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

BinOpr LuaParser::getBinaryOperator(LuaTokenType op)
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

void LuaParser::checkNext(LuaTokenType c, std::shared_ptr<LuaAstNode> parent)
{
	if (_tokenParser->Current().TokenType != c)
	{
		throw LuaParserException(format("token type {} expected", c));
	}

	parent->AddChild(createAstNodeFromToken(LuaAstNodeType::KeyWord, _tokenParser->Current()));

	_tokenParser->Next();
}

bool LuaParser::testNext(LuaTokenType c, std::shared_ptr<LuaAstNode> parent)
{
	if (_tokenParser->Current().TokenType == c)
	{
		parent->AddChild(createAstNodeFromToken(LuaAstNodeType::KeyWord, _tokenParser->Current()));
		_tokenParser->Next();
		return true;
	}
	return false;
}


std::shared_ptr<LuaAstNode> LuaParser::createAstNode(LuaAstNodeType type)
{
	return std::make_shared<LuaAstNode>(type, _tokenParser->GetSource().c_str());
}

std::shared_ptr<LuaAstNode> LuaParser::createAstNodeFromToken(LuaAstNodeType type, LuaToken& token)
{
	return std::make_shared<LuaAstNode>(type, token);
}

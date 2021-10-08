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
	if (testNext(TK_ELSE))
	{
		block(ifNode);
	}
	checkMatch(TK_END, TK_IF);

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

bool LuaParser::testNext(LuaTokenType type)
{
	if (_tokenParser->Current().TokenType == type)
	{
		_tokenParser->Next();
		return true;
	}
	else
	{
		return false;
	}
}

void LuaParser::block(std::shared_ptr<LuaAstNode> parent)
{
	auto blockNode = createAstNode(LuaAstNodeType::Block);

	statementList(blockNode);

	parent->AddChild(blockNode);
}

void LuaParser::checkMatch(LuaTokenType what, LuaTokenType who)
{
	if (!testNext(what))
	{
		throw LuaParserException(format("token {} expected ,(to close {} at", what, who));
	}
}

void LuaParser::expression(std::shared_ptr<LuaAstNode> parent)
{
	auto expressNode = createAstNode(LuaAstNodeType::Expression);
	subexpression(parent, 0);

	parent->AddChild(expressNode);
}

/*
** subexpr -> (simpleexp | unop subexpr) { binop subexpr }
** where 'binop' is any binary operator with a priority higher than 'limit'
*/
BinOpr LuaParser::subexpression(std::shared_ptr<LuaAstNode> expressNode, int limit)
{
	UnOpr uop = getUnaryOperator(_tokenParser->Current().TokenType);
	if (uop != UnOpr::OPR_NOUNOPR) /* prefix (unary) operator? */
	{
		auto unaryTokenNode = createAstNodeFromToken(LuaAstNodeType::UnaryOperator, _tokenParser->Current());
		expressNode->AddChild(unaryTokenNode);

		_tokenParser->Next();
		subexpression(expressNode, UNARY_PRIORITY);
	}
	else
	{
		simpleExpression(expressNode);
	}

	auto op = getBinaryOperator(_tokenParser->Current().TokenType);
	/* expand while operators have priorities higher than 'limit' */
	while(op != BinOpr::OPR_NOBINOPR && priority[static_cast<int>(op)].left > limit)
	{
		auto binaryOperator = createAstNodeFromToken(LuaAstNodeType::BinaryOperator, _tokenParser->Current());
		_tokenParser->Next();
		auto nextop = subexpression(expressNode, priority[static_cast<int>(op)].right);

		op = nextop;
	}

	return op; /* return first untreated operator */
}

/* simpleexp -> FLT | INT | STRING | NIL | TRUE | FALSE | ... |
				constructor | FUNCTION body | suffixedexp */
void LuaParser::simpleExpression(std::shared_ptr<LuaAstNode> expressNode)
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
			break;
		}
	case '{':
		{
			tableConstructor(expressNode);
			break;
		}
	case TK_FUNCTION:
		{
			auto closureNode = createAstNode(LuaAstNodeType::ClosureExpression);
			auto functionKeyWordNode = createAstNodeFromToken(LuaAstNodeType::KeyWord, _tokenParser->Current());
			closureNode->AddChild(functionKeyWordNode);

			_tokenParser->Next();
			functionBody(closureNode);

			expressNode->AddChild(closureNode);
			break;
		}
	default:
		{
			suffixedExpression(expressNode);
			return;
		}
	}
	_tokenParser->Next();
}

/* constructor -> '{' [ field { sep field } [sep] ] '}'
   sep -> ',' | ';'
*/
void LuaParser::tableConstructor(std::shared_ptr<LuaAstNode> expressNode)
{
}

void LuaParser::functionBody(std::shared_ptr<LuaAstNode> expressNode)
{
}

void LuaParser::suffixedExpression(std::shared_ptr<LuaAstNode> expressNode)
{
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

std::shared_ptr<LuaAstNode> LuaParser::createAstNode(LuaAstNodeType type)
{
	return std::make_shared<LuaAstNode>(type, _tokenParser->GetSource().c_str());
}

std::shared_ptr<LuaAstNode> LuaParser::createAstNodeFromToken(LuaAstNodeType type, LuaToken& token)
{
	return std::make_shared<LuaAstNode>(type, token);
}

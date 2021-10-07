#include "LuaParser/LuaParser.h"

#include "LuaParser/LuaOperatorType.h"
#include "Util/format.h"

LuaParser::LuaParser(std::shared_ptr<LuaTokenParser> tokenParser)
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

void LuaParser::statementList()
{
	while (blockFollow(true))
	{
		statement();
	}
}

void LuaParser::statement()
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
			ifStatement();
			break;
		}
	}
}

/* ifstat -> IF cond THEN block {ELSEIF cond THEN block} [ELSE block] END */
void LuaParser::ifStatement()
{
	testThenBlock();
	while (_tokenParser->Current().TokenType == TK_ELSEIF)
	{
		testThenBlock();
	}
	if (testNext(TK_ELSE))
	{
		block();
	}
	checkMatch(TK_END, TK_IF);
}

/* test_then_block -> [IF | ELSEIF] cond THEN block */
void LuaParser::testThenBlock()
{
	_tokenParser->Next(); /*skip if or elseif*/
	expression()
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

void LuaParser::block()
{
	statementList();
}

void LuaParser::checkMatch(LuaTokenType what, LuaTokenType who)
{
	if (!testNext(what))
	{
		throw LuaParserException(format("token {} expected ,(to close {} at", what, who));
	}
}

void LuaParser::expression()
{
	subexpression();
}

/*
** subexpr -> (simpleexp | unop subexpr) { binop subexpr }
** where 'binop' is any binary operator with a priority higher than 'limit'
*/
void LuaParser::subexpression()
{
	UnOpr uop = getUnoperator(_tokenParser->Current().TokenType);
	if(uop != OPR_NOUNOPR)
	{
		_tokenParser->Next();
		subexpression();
	}
	else
	{
		simpleExpression();
	}

	
}

UnOpr LuaParser::getUnoperator(LuaTokenType op)
{
	switch (op)
	{
	case TK_NOT:
		{
			return OPR_NOT;
		}
	case '-':
		{
			return OPR_MINUS;
		}
	case '~':
		{
			return OPR_BNOT;
		}
	case '#':
		{
			return OPR_LEN;
		}
	default:
		{
			return OPR_NOUNOPR;
		}
	}
}

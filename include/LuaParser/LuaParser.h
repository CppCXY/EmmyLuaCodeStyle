#pragma once

#include <memory>
#include "LuaTokenParser.h"
#include "LuaOperatorType.h"

class LuaParser
{
public:
	static std::shared_ptr<LuaParser> LoadFromFile(std::string_view filename);

	static std::shared_ptr<LuaParser> LoadFromBuffer(std::string&& buffer);

	bool Parse();

	struct LuaAstChunk GetAst();
private:
	LuaParser(std::shared_ptr<LuaTokenParser> tokenParser);

	bool blockFollow(bool withUntil = false);

	void statementList();

	void statement();

	void ifStatement();

	void testThenBlock();

	bool testNext(LuaTokenType type);

	void block();

	void checkMatch(LuaTokenType what, LuaTokenType who);

	void expression();

	void subexpression();

	UnOpr getUnoperator(LuaTokenType op);
	
	std::shared_ptr<LuaTokenParser> _tokenParser;


	
};



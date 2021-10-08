#pragma once

#include <memory>
#include "LuaTokenParser.h"
#include "LuaOperatorType.h"
#include "LuaAstNode.h"

class LuaParser
{
public:
	static std::shared_ptr<LuaParser> LoadFromFile(std::string_view filename);

	static std::shared_ptr<LuaParser> LoadFromBuffer(std::string&& buffer);

	bool Parse();

	std::shared_ptr<LuaAstNode> GetAst();
private:
	LuaParser(std::shared_ptr<LuaTokenParser> tokenParser);

	bool blockFollow(bool withUntil = false);

	void statementList(std::shared_ptr<LuaAstNode> blockNode);

	void statement(std::shared_ptr<LuaAstNode> blockNode);

	void ifStatement(std::shared_ptr<LuaAstNode> blockNode);

	void testThenBlock(std::shared_ptr<LuaAstNode> ifNode);

	bool testNext(LuaTokenType type);

	void block(std::shared_ptr<LuaAstNode> parent);

	void checkMatch(LuaTokenType what, LuaTokenType who);

	void expression(std::shared_ptr<LuaAstNode> parent);

	BinOpr subexpression(std::shared_ptr<LuaAstNode> expressNode, int limit);

	void simpleExpression(std::shared_ptr<LuaAstNode> expressNode);

	void tableConstructor(std::shared_ptr<LuaAstNode> expressNode);

	void functionBody(std::shared_ptr<LuaAstNode> expressNode);

	void suffixedExpression(std::shared_ptr<LuaAstNode> expressNode);

	UnOpr getUnaryOperator(LuaTokenType op);

	BinOpr getBinaryOperator(LuaTokenType op);

	std::shared_ptr<LuaAstNode> createAstNode(LuaAstNodeType type);

	std::shared_ptr<LuaAstNode> createAstNodeFromToken(LuaAstNodeType type, LuaToken& token);

	std::shared_ptr<LuaTokenParser> _tokenParser;

	std::shared_ptr<LuaAstNode> _chunkAstNode;


};



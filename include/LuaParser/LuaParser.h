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

	void whileStatement(std::shared_ptr<LuaAstNode> blockNode);

	void doStatement(std::shared_ptr<LuaAstNode> blockNode);

	void condition(std::shared_ptr<LuaAstNode> parent);

	void testThenBlock(std::shared_ptr<LuaAstNode> ifNode);

	void block(std::shared_ptr<LuaAstNode> parent);

	void checkMatch(LuaTokenType what, LuaTokenType who, std::shared_ptr<LuaAstNode> parent);

	void expressionList(std::shared_ptr<LuaAstNode> parent);

	void expression(std::shared_ptr<LuaAstNode> parent);

	void subexpression(std::shared_ptr<LuaAstNode> expressionNode, int limit);

	void simpleExpression(std::shared_ptr<LuaAstNode> expressionNode);

	void tableConstructor(std::shared_ptr<LuaAstNode> expressionNode);

	void field(std::shared_ptr<LuaAstNode> tableExpressionNode);

	void listField(std::shared_ptr<LuaAstNode> tableExpressionNode);

	void rectField(std::shared_ptr<LuaAstNode> tableExpressionNode);

	void functionBody(std::shared_ptr<LuaAstNode> closureExpression);

	void paramList(std::shared_ptr<LuaAstNode> functionBodyNode);

	void suffixedExpression(std::shared_ptr<LuaAstNode> expressionNode);

	void functionCallArgs(std::shared_ptr<LuaAstNode> expressionNode);

	void fieldSel(std::shared_ptr<LuaAstNode> expressionNode);

	void yIndex(std::shared_ptr<LuaAstNode> expressionNode);

	void primaryExpression(std::shared_ptr<LuaAstNode> expressionNode);

	UnOpr getUnaryOperator(LuaTokenType op);

	BinOpr getBinaryOperator(LuaTokenType op);

	/*
	 * 他是检查当前token的type是否与c相同
	 * 如果是就跳过当前,
	 * 否则会抛出异常
	 */
	void checkNext(LuaTokenType c, std::shared_ptr<LuaAstNode> parent);

	/*
	 * 他是检查当前token的type是否与c相同
	 * 如果是就跳过当前，并返回true
	 * 否则返回false
	 */
	bool testNext(LuaTokenType c, std::shared_ptr<LuaAstNode> parent);

	void codeName(std::shared_ptr<LuaAstNode> parent);

	std::shared_ptr<LuaAstNode> createAstNode(LuaAstNodeType type);

	std::shared_ptr<LuaAstNode> createAstNodeFromToken(LuaAstNodeType type, LuaToken& token);

	std::shared_ptr<LuaAstNode> createAstNodeFromCurrentToken(LuaAstNodeType type);

	std::shared_ptr<LuaTokenParser> _tokenParser;

	std::shared_ptr<LuaAstNode> _chunkAstNode;


};




#pragma once

#include <memory>
#include "LuaTokenParser.h"
#include "LuaOperatorType.h"
#include "LuaAstNode.h"
#include "LuaAttribute.h"

class LuaParser
{
public:
	static std::shared_ptr<LuaParser> LoadFromFile(std::string_view filename);

	static std::shared_ptr<LuaParser> LoadFromBuffer(std::string&& buffer);

	LuaParser(std::shared_ptr<LuaTokenParser> tokenParser);

	bool Parse();

	std::shared_ptr<LuaAstNode> GetAst();

	std::vector<LuaError>& GetErrors();

	bool HasError() const;
private:
	void buildAstWithComment();

	bool blockFollow(bool withUntil = false);

	void statementList(std::shared_ptr<LuaAstNode> blockNode);

	void statement(std::shared_ptr<LuaAstNode> blockNode);

	void ifStatement(std::shared_ptr<LuaAstNode> blockNode);

	void whileStatement(std::shared_ptr<LuaAstNode> blockNode);

	void doStatement(std::shared_ptr<LuaAstNode> blockNode);

	void forStatement(std::shared_ptr<LuaAstNode> blockNode);

	void repeatStatement(std::shared_ptr<LuaAstNode> blockNode);

	void functionStatement(std::shared_ptr<LuaAstNode> blockNode);

	void localFunctionStatement(std::shared_ptr<LuaAstNode> blockNode);

	void localStatement(std::shared_ptr<LuaAstNode> blockNode);

	void LabelStatement(std::shared_ptr<LuaAstNode> blockNode);

	void returnStatement(std::shared_ptr<LuaAstNode> blockNode);

	void breakStatement(std::shared_ptr<LuaAstNode> blockNode);

	void gotoStatement(std::shared_ptr<LuaAstNode> blockNode);

	void expressionStatement(std::shared_ptr<LuaAstNode> blockNode);

	// 赋值语句由表达式语句转换而来
	void assignStatement(std::shared_ptr<LuaAstNode> assignStatementNode);

	void forNumber(std::shared_ptr<LuaAstNode> forStatement);

	void forList(std::shared_ptr<LuaAstNode> forStatement);

	void forBody(std::shared_ptr<LuaAstNode> forNode);

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

	void functionName(std::shared_ptr<LuaAstNode> functionNode);

	std::string_view checkName(std::shared_ptr<LuaAstNode> parent);

	LuaAttribute getLocalAttribute(std::shared_ptr<LuaAstNode> nameDefList);

	void check(LuaTokenType c);

	void primaryExpression(std::shared_ptr<LuaAstNode> expressionNode);

	UnOpr getUnaryOperator(LuaTokenType op);

	BinOpr getBinaryOperator(LuaTokenType op);

	/*
	 * 他是检查当前token的type是否与c相同
	 * 如果是就跳过当前,
	 * 否则会抛出异常
	 */
	void checkAndNext(LuaTokenType c, std::shared_ptr<LuaAstNode> parent, LuaAstNodeType addType = LuaAstNodeType::KeyWord);

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

	void luaError(std::string_view message, std::shared_ptr<LuaAstNode> parent);

	void luaMatchError(std::string message, TextRange range);

	std::shared_ptr<LuaTokenParser> _tokenParser;

	std::shared_ptr<LuaAstNode> _chunkAstNode;

	std::vector<LuaError> _errors;
};





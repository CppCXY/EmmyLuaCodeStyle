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

	void BuildAst();

	void BuildAstWithComment();

	std::shared_ptr<LuaAstNode> GetAst();

	std::vector<LuaError>& GetErrors();

	bool HasError() const;

	std::vector<LuaToken>& GetAllComments();

	int GetLine(int offset) const;

	int GetColumn(int offset) const;

	int GetTotalLine();

	std::string_view GetSource();

	bool IsEmptyLine(int line);

	void SetFilename(std::string_view filename);

	std::string_view GetFilename();

	std::shared_ptr<LuaFile> GetLuaFile();
private:
	bool BlockFollow(bool withUntil = false);

	void StatementList(std::shared_ptr<LuaAstNode> blockNode);

	void Statement(std::shared_ptr<LuaAstNode> blockNode);

	void IfStatement(std::shared_ptr<LuaAstNode> blockNode);

	void WhileStatement(std::shared_ptr<LuaAstNode> blockNode);

	void DoStatement(std::shared_ptr<LuaAstNode> blockNode);

	void ForStatement(std::shared_ptr<LuaAstNode> blockNode);

	void RepeatStatement(std::shared_ptr<LuaAstNode> blockNode);

	void FunctionStatement(std::shared_ptr<LuaAstNode> blockNode);

	void LocalFunctionStatement(std::shared_ptr<LuaAstNode> blockNode);

	void LocalStatement(std::shared_ptr<LuaAstNode> blockNode);

	void LabelStatement(std::shared_ptr<LuaAstNode> blockNode);

	void ReturnStatement(std::shared_ptr<LuaAstNode> blockNode);

	void BreakStatement(std::shared_ptr<LuaAstNode> blockNode);

	void GotoStatement(std::shared_ptr<LuaAstNode> blockNode);

	void ExpressionStatement(std::shared_ptr<LuaAstNode> blockNode);

	void AssignStatement(std::shared_ptr<LuaAstNode> expressionListNode, std::shared_ptr<LuaAstNode> assignStatementNode);

	void ForNumber(std::shared_ptr<LuaAstNode> forStatement);

	void ForList(std::shared_ptr<LuaAstNode> forStatement);

	void ForBody(std::shared_ptr<LuaAstNode> forNode);

	void Condition(std::shared_ptr<LuaAstNode> parent);

	void TestThenBlock(std::shared_ptr<LuaAstNode> ifNode);

	void Block(std::shared_ptr<LuaAstNode> parent);

	void CheckMatch(LuaTokenType what, LuaTokenType who, std::shared_ptr<LuaAstNode> parent,
	                LuaAstNodeType addType = LuaAstNodeType::KeyWord);

	void ExpressionList(std::shared_ptr<LuaAstNode> parent);

	void Expression(std::shared_ptr<LuaAstNode> parent);

	void Subexpression(std::shared_ptr<LuaAstNode> expressionNode, int limit);

	void SimpleExpression(std::shared_ptr<LuaAstNode> expressionNode);

	void TableConstructor(std::shared_ptr<LuaAstNode> expressionNode);

	void Field(std::shared_ptr<LuaAstNode> tableExpressionNode);

	void ListField(std::shared_ptr<LuaAstNode> tableFieldNode);

	void RectField(std::shared_ptr<LuaAstNode> tableFieldNode);

	void FunctionBody(std::shared_ptr<LuaAstNode> closureExpression);

	void ParamList(std::shared_ptr<LuaAstNode> functionBodyNode);

	void SuffixedExpression(std::shared_ptr<LuaAstNode> expressionNode);

	void FunctionCallArgs(std::shared_ptr<LuaAstNode>& expressionNode);

	// 该函数会改变入参
	void FieldSel(std::shared_ptr<LuaAstNode>& expressionNode);

	void YIndex(std::shared_ptr<LuaAstNode> expressionNode);

	void FunctionName(std::shared_ptr<LuaAstNode> functionNode);

	std::string_view CheckName(std::shared_ptr<LuaAstNode> parent);

	LuaAttribute GetLocalAttribute(std::shared_ptr<LuaAstNode> nameDefList);

	void Check(LuaTokenType c);

	void PrimaryExpression(std::shared_ptr<LuaAstNode> primaryExpression);

	UnOpr GetUnaryOperator(LuaTokenType op);

	BinOpr GetBinaryOperator(LuaTokenType op);

	/*
	 * 他是检查当前token的type是否与c相同
	 * 如果是就跳过当前,
	 * 否则会生成错误
	 */
	void CheckAndNext(LuaTokenType c, std::shared_ptr<LuaAstNode> parent,
	                  LuaAstNodeType addType = LuaAstNodeType::KeyWord);

	/*
	 * 他是检查当前token的type是否与c相同
	 * 如果是就跳过当前，并返回true
	 * 否则返回false
	 */
	bool TestNext(LuaTokenType c, std::shared_ptr<LuaAstNode> parent, LuaAstNodeType addType = LuaAstNodeType::KeyWord);

	void CodeName(std::shared_ptr<LuaAstNode> parent);

	std::shared_ptr<LuaAstNode> CreateAstNode(LuaAstNodeType type);

	std::shared_ptr<LuaAstNode> CreateAstNodeFromToken(LuaAstNodeType type, LuaToken& token);

	std::shared_ptr<LuaAstNode> CreateAstNodeFromCurrentToken(LuaAstNodeType type);

	void ThrowLuaError(std::string_view message, std::shared_ptr<LuaAstNode> parent);

	void ThrowMatchError(std::string message, TextRange range);

	std::shared_ptr<LuaTokenParser> _tokenParser;

	std::shared_ptr<LuaAstNode> _chunkAstNode;

	std::vector<LuaError> _errors;

	std::shared_ptr<LuaFile> _file;
};

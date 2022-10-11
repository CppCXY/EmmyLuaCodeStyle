#pragma once

#include <memory>
#include <vector>
#include "LuaParser/Lexer/LuaLexer.h"
#include "LuaOperatorType.h"
#include "LuaAttribute.h"
#include "LuaParseError.h"
#include "LuaParser/Ast/LuaAstNode.h"
#include "LuaParser/Ast/LuaAstTree.h"
#include "Mark.h"

class LuaParser
{
public:
	static std::shared_ptr<LuaParser> LoadFromFile(std::string_view filename);

	static std::shared_ptr<LuaParser> LoadFromBuffer(std::string&& buffer);

	LuaParser(std::shared_ptr<LuaFile> luaFile, std::vector<LuaToken>&& tokens);

    bool Parse();

    std::vector<MarkEvent>& GetEvents();

    std::vector<LuaToken>& GetTokens();

	std::vector<LuaParseError>& GetErrors();

	bool HasError() const;

	std::shared_ptr<LuaFile> GetLuaFile();

    Marker Mark();

private:
    void Next();

    LuaTokenType LookAhead();

    LuaTokenType Current();

    void SkipComment();

	bool BlockFollow(bool rightbrace = false);

	void StatementList();

	void Statement();

	void IfStatement();

	void WhileStatement();

	void DoStatement();

	void ForStatement();

	void RepeatStatement();

	void FunctionStatement();

	void LocalFunctionStatement();

	void LocalStatement();

	void LabelStatement();

	void ReturnStatement();

	void BreakStatement();

	void GotoStatement();

	void ExpressionStatement();

	void ForNumber();

	void ForList();

	void ForBody();

	void Condition();

	void TestThenBlock();

    void NameDefList();

	void Block();

	void ExpressionList( LuaTokenType stopToken = 0);

	void Expression();

	void Subexpression(int limit);

    CompleteMarker SimpleExpression();

    CompleteMarker TableConstructor();

	void Field();

	void ListField();

	void RectField();

	void FunctionBody();

	void ParamList();

    CompleteMarker SuffixedExpression();

	void FunctionCallArgs();

	void FieldSel();

	void YIndex();

	void FunctionName();

	std::string_view CheckName();

	void LocalAttribute();

	void Check(LuaTokenType c);

    void PrimaryExpression();

	UnOpr GetUnaryOperator(LuaTokenType op);

	BinOpr GetBinaryOperator(LuaTokenType op);

	/*
	 * 他是检查当前token的type是否与c相同
	 * 如果是就跳过当前,
	 * 否则会生成错误
	 */
	void CheckAndNext(LuaTokenType c);

	/*
	 * 他是检查当前token的type是否与c相同
	 * 如果是就跳过当前，并返回true
	 * 否则返回false
	 */
	bool TestAndNext(LuaTokenType c);

	void ThrowLuaError(std::string_view message);

	void ThrowMatchError(std::string message);

    std::vector<LuaToken> _tokens;
    std::size_t _tokenIndex;
	std::vector<LuaParseError> _errors;
	std::shared_ptr<LuaFile> _file;
    std::vector<MarkEvent> _events;
    bool _invalid;
    LuaTokenType _current;
};

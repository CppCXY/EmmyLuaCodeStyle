#pragma once
#include <memory>
#include <sstream>
#include "LuaParser/LuaParser.h"
#include "LuaFormatOptions.h"
#include "LuaCodeDiagnosis.h"
#include "FormatElement/FormatElement.h"

class LuaFormatter
{
public:
	LuaFormatter(std::shared_ptr<LuaParser> luaParser, LuaFormatOptions& options);
	~LuaFormatter() = default;

	void BuildFormattedElement();

	std::string GetFormattedText();

	void AddDiagnosis(TextRange range, std::string_view message);

	std::vector<LuaCodeDiagnosis>& GetDiagnosis();

protected:
	std::shared_ptr<FormatElement> FormatNode(std::shared_ptr<LuaAstNode> node);

	std::shared_ptr<FormatElement> FormatBlock(std::shared_ptr<LuaAstNode> blockNode);

	std::shared_ptr<FormatElement> FormatLocalStatement(std::shared_ptr<LuaAstNode> localStatement);

	std::shared_ptr<FormatElement> FormatAssignment(std::shared_ptr<LuaAstNode> assignStatement);

	std::shared_ptr<FormatElement> FormatNameDefList(std::shared_ptr<LuaAstNode> nameDefList);

	std::shared_ptr<FormatElement> FormatExpressionList(std::shared_ptr<LuaAstNode> expressionList);

	std::shared_ptr<FormatElement> FormatComment(std::shared_ptr<LuaAstNode> comment);

	std::shared_ptr<FormatElement> FormatBreakStatement(std::shared_ptr<LuaAstNode> breakNode);

	std::shared_ptr<FormatElement> FormatReturnStatement(std::shared_ptr<LuaAstNode> returnNode);

	std::shared_ptr<FormatElement> FormatGotoStatement(std::shared_ptr<LuaAstNode> gotoNode);

	std::shared_ptr<FormatElement> FormatLabelStatement(std::shared_ptr<LuaAstNode> labelStatement);

	std::shared_ptr<FormatElement> FormatIdentify(std::shared_ptr<LuaAstNode> identify);

	std::shared_ptr<FormatElement> FormatGeneralOperator(std::shared_ptr<LuaAstNode> general);

	std::shared_ptr<FormatElement> FormatKeyWords(std::shared_ptr<LuaAstNode> keywords);

	std::shared_ptr<FormatElement> FormatDoStatement(std::shared_ptr<LuaAstNode> doStatement);

	std::shared_ptr<FormatElement> FormatWhileStatement(std::shared_ptr<LuaAstNode> whileStatement);

	std::shared_ptr<FormatElement> FormatForStatement(std::shared_ptr<LuaAstNode> forStatement);

	std::shared_ptr<FormatElement> FormatForBody(std::shared_ptr<LuaAstNode> forBody);

	std::shared_ptr<FormatElement> FormatAttribute(std::shared_ptr<LuaAstNode> attribute);

	std::shared_ptr<FormatElement> FormatRepeatStatement(std::shared_ptr<LuaAstNode> repeatStatement);

	std::shared_ptr<FormatElement> FormatIfStatement(std::shared_ptr<LuaAstNode> ifStatement);

	std::shared_ptr<FormatElement> FormatExpressionStatement(std::shared_ptr<LuaAstNode> expressionStatement);

	std::shared_ptr<FormatElement> FormatExpression(std::shared_ptr<LuaAstNode> expression);

	void DefaultHandle(std::shared_ptr<LuaAstNode> node, std::shared_ptr<FormatElement> envElement);
private:

	std::shared_ptr<LuaParser> _parser;
	// std::shared_ptr<LuaFormatContext> _ctx;
	LuaFormatOptions _options;
	std::vector<LuaCodeDiagnosis> _diagnosis;
	std::shared_ptr<FormatElement> _env;
};








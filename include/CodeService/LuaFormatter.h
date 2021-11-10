#pragma once
#include <memory>
#include <sstream>
#include "LuaParser/LuaParser.h"
#include "LuaFormatOptions.h"
#include "FormatElement/FormatElement.h"
#include "LuaDiagnosisInfo.h"

class LuaFormatter
{
public:
	LuaFormatter(std::shared_ptr<LuaParser> luaParser, LuaFormatOptions& options);
	~LuaFormatter() = default;

	void BuildFormattedElement();

	std::string GetFormattedText();

	std::vector<LuaDiagnosisInfo> GetDiagnosisInfos();

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

	std::shared_ptr<FormatElement> FormatCallArgList(std::shared_ptr<LuaAstNode> callArgList);

	std::shared_ptr<FormatElement> FormatFunctionStatement(std::shared_ptr<LuaAstNode> functionStatement);

	std::shared_ptr<FormatElement> FormatNameExpression(std::shared_ptr<LuaAstNode> nameExpression);

	std::shared_ptr<FormatElement> FormatFunctionBody(std::shared_ptr<LuaAstNode> functionBody);

	std::shared_ptr<FormatElement> FormatParamList(std::shared_ptr<LuaAstNode> paramList);

	std::shared_ptr<FormatElement> FormatLocalFunctionStatement(std::shared_ptr<LuaAstNode> localFunctionStatement);

	std::shared_ptr<FormatElement> FormatClosureExpression(std::shared_ptr<LuaAstNode> closureExpression);

	std::shared_ptr<FormatElement> FormatTableExpression(std::shared_ptr<LuaAstNode> tableExpression);

	std::shared_ptr<FormatElement> FormatTableField(std::shared_ptr<LuaAstNode> tableField);

	void DefaultHandle(std::shared_ptr<LuaAstNode> node, std::shared_ptr<FormatElement> envElement);

	// 以下是特殊格式化规则
	// 规则1 连续赋值语句会试图检测并对齐等号
	std::shared_ptr<FormatElement> FormatAlignStatement(int& currentIndex,const std::vector<std::shared_ptr<LuaAstNode>>& vec);

	std::shared_ptr<FormatElement> FormatAlignTableField(int& currentIndex,const std::vector<std::shared_ptr<LuaAstNode>>& vec);
	//意思其实是格式化任意节点加上Block和可选的接受一个end
	std::shared_ptr<FormatElement> FormatNodeAndBlockOrEnd(int& currentIndex, const std::vector<std::shared_ptr<LuaAstNode>>& vec);
	//当前索引必须是已经消耗过的索引
	std::shared_ptr<FormatElement> FormatBlockFromParent(int& currentIndex, const std::vector<std::shared_ptr<LuaAstNode>>& vec);

	// 以下是表达式相关，表达式会联合布局
	void FormatSubExpressionNode(std::shared_ptr<LuaAstNode> expression, std::shared_ptr<FormatElement> env);

	std::shared_ptr<FormatElement> FormatExpression(std::shared_ptr<LuaAstNode> expression, std::shared_ptr<FormatElement> env = nullptr);

	std::shared_ptr<FormatElement> FormatBinaryExpression(std::shared_ptr<LuaAstNode> binaryExpression);

	std::shared_ptr<FormatElement> FormatUnaryExpression(std::shared_ptr<LuaAstNode> unaryExpression);

	std::shared_ptr<FormatElement> FormatPrimaryExpression(std::shared_ptr<LuaAstNode> primaryExpression);

	std::shared_ptr<FormatElement> FormatIndexExpression(std::shared_ptr<LuaAstNode> indexExpression);

	std::shared_ptr<FormatElement> FormatCallExpression(std::shared_ptr<LuaAstNode> callExpression);
private:

	std::shared_ptr<LuaParser> _parser;
	LuaFormatOptions _options;
	std::shared_ptr<FormatElement> _env;
};

















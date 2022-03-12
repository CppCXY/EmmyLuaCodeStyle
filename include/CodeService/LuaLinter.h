#include "LuaLinterOptions.h"
#include "FormatElement/FormatElement.h"

class LuaLinter
{
	LuaLinter(std::shared_ptr<LuaParser> luaParser, LuaLinterOptions& options);

	~LuaLinter() = default;

	void BuildDiagnosisElement();

	std::vector<LuaDiagnosisInfo> GetDiagnoseResult();
protected:
	std::shared_ptr<FormatElement> DiagnoseNode(std::shared_ptr<LuaAstNode> block);
	std::shared_ptr<FormatElement> DiagnoseBlock(std::shared_ptr<LuaAstNode> block);
	std::shared_ptr<FormatElement> DiagnoseLocalStatement(std::shared_ptr<LuaAstNode> localStatement);
	std::shared_ptr<FormatElement> DiagnoseAssignmentStatement(std::shared_ptr<LuaAstNode> assignStatement);
	std::shared_ptr<FormatElement> DiagnoseRepeatStatement(std::shared_ptr<LuaAstNode> repeatStatement);
	std::shared_ptr<FormatElement> DiagnoseDoStatement(std::shared_ptr<LuaAstNode> doStatement);
	std::shared_ptr<FormatElement> DiagnoseIfStatement(std::shared_ptr<LuaAstNode> ifStatement);
	std::shared_ptr<FormatElement> DiagnoseForStatement(std::shared_ptr<LuaAstNode> forStatement);
	std::shared_ptr<FormatElement> DiagnoseForBody(std::shared_ptr<LuaAstNode> forBody);
	std::shared_ptr<FormatElement> DiagnoseWhileStatement(std::shared_ptr<LuaAstNode> whileStatement);
	std::shared_ptr<FormatElement> DiagnoseBreakStatement(std::shared_ptr<LuaAstNode> breakStatement);
	std::shared_ptr<FormatElement> DiagnoseReturnStatement(std::shared_ptr<LuaAstNode> returnStatement);
	std::shared_ptr<FormatElement> DiagnoseGotoStatement(std::shared_ptr<LuaAstNode> gotoStatement);
	std::shared_ptr<FormatElement> DiagnoseExpressionStatement(std::shared_ptr<LuaAstNode> expressionStatement);
	std::shared_ptr<FormatElement> DiagnoseFunctionStatement(std::shared_ptr<LuaAstNode> functionStatement);
	std::shared_ptr<FormatElement> DiagnoseLocalFunctionStatement(std::shared_ptr<LuaAstNode> localFunctionStatement);
	std::shared_ptr<FormatElement> DiagnoseFunctionBody(std::shared_ptr<LuaAstNode> functionBody);
	std::shared_ptr<FormatElement> DiagnoseNameExpression(std::shared_ptr<LuaAstNode> nameExpression);
	std::shared_ptr<FormatElement> DiagnoseLabelStatement(std::shared_ptr<LuaAstNode> labelStatement);
	std::shared_ptr<FormatElement> DiagnoseParamList(std::shared_ptr<LuaAstNode> paramList);

	std::shared_ptr<FormatElement> DiagnoseExpression(std::shared_ptr<LuaAstNode> expressionStatement,
	                                                  std::shared_ptr<FormatElement> env = nullptr);
	std::shared_ptr<FormatElement> DiagnoseExpressionList(std::shared_ptr<LuaAstNode> expressionList,
	                                                      std::shared_ptr<FormatElement> env = nullptr);

	void DiagnoseSubExpression(std::shared_ptr<LuaAstNode> expression, std::shared_ptr<FormatElement> env);

	std::shared_ptr<FormatElement> DiagnoseBinaryExpression(std::shared_ptr<LuaAstNode> binaryExpression);

	std::shared_ptr<FormatElement> DiagnoseUnaryExpression(std::shared_ptr<LuaAstNode> unaryExpression);

	std::shared_ptr<FormatElement> DiagnosePrimaryExpression(std::shared_ptr<LuaAstNode> primaryExpression);

	std::shared_ptr<FormatElement> DiagnoseIndexExpression(std::shared_ptr<LuaAstNode> indexExpression);

	std::shared_ptr<FormatElement> DiagnoseCallExpression(std::shared_ptr<LuaAstNode> callExpression);

	std::shared_ptr<FormatElement> DiagnoseTableAppendExpression(std::shared_ptr<LuaAstNode> expression);



	std::shared_ptr<FormatElement> DiagnoseAlignStatement(LuaAstNode::ChildIterator& it,
	                                                      const LuaAstNode::ChildrenContainer& children);
	std::shared_ptr<FormatElement> DiagnoseNodeAndBlockOrEnd(LuaAstNode::ChildIterator& it,
	                                                         const LuaAstNode::ChildrenContainer& children);
	std::shared_ptr<FormatElement> CalculateBlockFromParent(LuaAstNode::ChildIterator& it,
	                                                        const LuaAstNode::ChildrenContainer& children);

	void DefaultHandle(std::shared_ptr<LuaAstNode> node, std::shared_ptr<FormatElement> env);
private:
	std::shared_ptr<LuaParser> _parser;
	LuaLinterOptions& _options;
	std::shared_ptr<FormatElement> _env;
};

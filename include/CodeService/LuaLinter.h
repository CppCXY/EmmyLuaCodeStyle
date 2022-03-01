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


	std::shared_ptr<FormatElement> DiagnoseExpression(std::shared_ptr<LuaAstNode> expressionStatement, std::shared_ptr<FormatElement> env = nullptr);
	std::shared_ptr<FormatElement> DiagnoseExpressionList(std::shared_ptr<LuaAstNode> expressionList, std::shared_ptr<FormatElement> env = nullptr);

	std::shared_ptr<FormatElement> DiagnoseAlignStatement(LuaAstNode::ChildIterator& it,
	                                                      const LuaAstNode::ChildrenContainer& children);
	std::shared_ptr<FormatElement> DiagnoseNodeAndBlockOrEnd(LuaAstNode::ChildIterator& it,
	                                                         const LuaAstNode::ChildrenContainer& children);

	void DefaultHandle(std::shared_ptr<LuaAstNode> node, std::shared_ptr<FormatElement> env);
private:
	std::shared_ptr<LuaParser> _parser;
	LuaLinterOptions& _options;
	std::shared_ptr<FormatElement> _env;
};

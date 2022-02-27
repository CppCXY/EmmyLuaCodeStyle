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
	std::shared_ptr<FormatElement> DiagnoseBreakStatement(std::shared_ptr<LuaAstNode> whileStatement);
	std::shared_ptr<FormatElement> DiagnoseReturnStatement(std::shared_ptr<LuaAstNode> whileStatement);
	std::shared_ptr<FormatElement> DiagnoseGotoStatement(std::shared_ptr<LuaAstNode> whileStatement);
	std::shared_ptr<FormatElement> DiagnoseExpressionStatement(std::shared_ptr<LuaAstNode> whileStatement);



	std::shared_ptr<FormatElement> DiagnoseAlignStatement(LuaAstNode::ChildIterator& it,
	                                                      const LuaAstNode::ChildrenContainer& children);

private:
	std::shared_ptr<LuaParser> _parser;
	LuaLinterOptions& _options;
	std::shared_ptr<FormatElement> _env;
};


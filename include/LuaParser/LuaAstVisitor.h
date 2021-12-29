#pragma once

#include <memory>
#include "LuaAstNode.h"

class LuaAstVisitor
{
public:
	virtual ~LuaAstVisitor();
	void Visit(std::shared_ptr<LuaAstNode> node);

protected:
	virtual void VisitBlock(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitLocalStatement(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitNameDefList(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitExpressionList(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitComment(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitLabelStatement(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitBreakStatement(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitGotoStatement(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitReturnStatement(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitIdentify(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitNameIdentify(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitGeneralOperator(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitKeyWords(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitWhileStatement(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitForStatement(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitForBody(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitAttribute(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitRepeatStatement(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitDoStatement(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitAssignment(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitIfStatement(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitExpressionStatement(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitExpression(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitCallExpression(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitCallArgList(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitFunctionStatement(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitLocalFunctionStatement(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitClosureExpression(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitNameExpression(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitTableExpression(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitTableField(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitParamList(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitFunctionBody(const std::shared_ptr<LuaAstNode>& node);
	virtual void VisitLiteralExpression(const std::shared_ptr<LuaAstNode>& node);
};

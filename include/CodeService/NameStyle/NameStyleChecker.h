#pragma once

#include <vector>
#include "LuaParser/LuaAstVisitor.h"
#include "CodeService/FormatElement/DiagnosisContext.h"
#include "NameDefineType.h"
#include "CheckElement.h"

class NameStyleChecker : public LuaAstVisitor
{
public:
	class Scope
	{
	public:
		std::map<std::string, std::shared_ptr<CheckElement>, std::less<>> LocalVariableMap;
	};

	NameStyleChecker(DiagnosisContext& ctx);

	void Analysis();

protected:
	void VisitLocalStatement(const std::shared_ptr<LuaAstNode>& node) override;
	void VisitParamList(const std::shared_ptr<LuaAstNode>& node) override;
	void VisitLocalFunctionStatement(const std::shared_ptr<LuaAstNode>& node) override;
	void VisitFunctionStatement(const std::shared_ptr<LuaAstNode>& node) override;
	void VisitAssignment(const std::shared_ptr<LuaAstNode>& node) override;
	void VisitTableField(const std::shared_ptr<LuaAstNode>& node) override;
	void VisitReturnStatement(const std::shared_ptr<LuaAstNode>& node) override;
	void VisitBlock(const std::shared_ptr<LuaAstNode>& node) override;
private:
	void RecordLocalVariable(std::shared_ptr<CheckElement> checkElement);
	bool InTopBlock(std::shared_ptr<LuaAstNode> chunkBlockStatement);
	bool IsGlobal(std::shared_ptr<LuaAstNode> node);

	static std::set<std::string, std::less<>> TableFieldSpecialName;
	static std::set<std::string, std::less<>> GlobalSpecialName;

	DiagnosisContext& _ctx;

	// block到作用域的映射
	std::map<std::shared_ptr<LuaAstNode>, Scope> _scopeMap;
	std::vector<std::shared_ptr<CheckElement>> _nameStyleCheckVector;
	std::string_view _moduleName;
};

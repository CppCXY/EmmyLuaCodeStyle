#pragma once

#include <vector>
#include "LuaParser/LuaAstVisitor.h"
#include "CodeService/FormatElement/DiagnosisContext.h"
#include "NameDefineType.h"

class NameStyleChecker : public LuaAstVisitor
{
public:
	class CheckElement
	{
	public:
		NameDefineType Type;
		std::shared_ptr<LuaAstNode> Node;
		std::shared_ptr<LuaAstNode> ExtraInfoNode = nullptr;
	};

	class Scope
	{
	public:
		std::map<std::string, std::shared_ptr<CheckElement>, std::less<>> LocalVariableMap;
	};

	NameStyleChecker(DiagnosisContext& ctx);

	std::vector<LuaDiagnosisInfo> Analysis();

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
	bool SnakeCase(std::string_view source);
	bool PascalCase(std::string_view source);
	bool CamelCase(std::string_view source);

	DiagnosisContext& _ctx;

	// block到作用域的映射
	std::map<std::shared_ptr<LuaAstNode>, Scope> _scopeMap;
	std::vector<std::shared_ptr<CheckElement>> _nameStyleCheckVector;
	std::string_view _moduleName;

};

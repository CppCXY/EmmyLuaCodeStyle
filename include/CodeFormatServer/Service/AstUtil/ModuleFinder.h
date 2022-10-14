#pragma once

#include <map>
#include <set>

#include "CodeService/Config/LuaCodeStyleOptions.h"
#include "LuaParser/LuaParser.h"
#include "LuaParser/LuaAstVisitor.h"

class ModuleFinder : public LuaAstVisitor
{
public:
	class Scope
	{
	public:
		std::set<std::string, std::less<>> LocalNameDefineSet;
	};

	ModuleFinder();

	void Analysis(std::shared_ptr<LuaParser> parser);

	std::vector<std::shared_ptr<LuaAstNode>>& GetUndefinedModule();

	std::set<std::string> GetDefinedName(std::shared_ptr<LuaAstNode> untilNode);

protected:
	void VisitParamList(const std::shared_ptr<LuaAstNode>& paramList) override;

	void VisitLocalFunctionStatement(const std::shared_ptr<LuaAstNode>& localFunctionStatement) override;

	void VisitLocalStatement(const std::shared_ptr<LuaAstNode>& localStatement) override;

	void VisitAssignment(const std::shared_ptr<LuaAstNode>& assignStatement) override;

	void VisitBlock(const std::shared_ptr<LuaAstNode>& block) override;

	void VisitNameIdentify(const std::shared_ptr<LuaAstNode>& node) override;

	void RecordLocalVariable(std::shared_ptr<LuaAstNode> node);

	bool IsUndefinedVariable(const std::shared_ptr<LuaAstNode>& node);

	std::shared_ptr<LuaCodeStyleOptions> _options;
	std::map<std::shared_ptr<LuaAstNode>, Scope> _scopeMap;
	std::vector<std::shared_ptr<LuaAstNode>> _undefinedModule;
};

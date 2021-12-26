#include "CodeFormatServer/Service/ModuleService.h"
#include "LuaParser/LuaAstVisitor.h"

class UnResolveModuleFinder : public LuaAstVisitor
{
protected:
	void VisitParamList(const std::shared_ptr<LuaAstNode>& node) override;

	void VisitLocalFunctionStatement(const std::shared_ptr<LuaAstNode>& node) override;

	void VisitLocalStatement(const std::shared_ptr<LuaAstNode>& node) override;

	void VisitIdentify(const std::shared_ptr<LuaAstNode>& node) override;

};




std::vector<vscode::Diagnostic> ModuleService::GetModuleDiagnostics()
{
}

#include "CodeFormatServer/Service/ModuleService.h"
#include "LuaParser/LuaAstVisitor.h"

class UnResolveModuleFinder : public LuaAstVisitor
{
public:
	class Scope
	{
	public:
		std::set<std::string, std::less<>> LocalVariableSet;
	};

	UnResolveModuleFinder(std::vector<vscode::Diagnostic>& result, std::shared_ptr<LuaCodeStyleOptions> options)
		: _result(result),
		  _options(options)
	{
	}

	void Find()
	{
		
	}

protected:
	void VisitParamList(const std::shared_ptr<LuaAstNode>& node) override
	{
	}

	void VisitLocalFunctionStatement(const std::shared_ptr<LuaAstNode>& node) override
	{
	}

	void VisitLocalStatement(const std::shared_ptr<LuaAstNode>& node) override
	{
	}

	void VisitExpression(const std::shared_ptr<LuaAstNode>& expression) override
	{
		if(expression->GetChildren().empty())
		switch (expression->GetType())
		{
			// case 



		}

	}

private:
	std::vector<vscode::Diagnostic>& _result;
	std::shared_ptr<LuaCodeStyleOptions> _options;
};

std::vector<vscode::Diagnostic> ModuleService::GetModuleDiagnostics(std::shared_ptr<LuaParser> parser,
                                                                    std::shared_ptr<LuaCodeStyleOptions> options)
{
	std::vector<vscode::Diagnostic> result;
	UnResolveModuleFinder finder(result, options);
	auto ast = parser->GetAst();

	ast->AcceptChildren(finder);

	return result;
}

std::vector<std::string> ModuleService::GetCompleteItems()
{
	return {};
}

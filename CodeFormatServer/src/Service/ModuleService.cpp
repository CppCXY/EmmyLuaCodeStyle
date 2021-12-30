#include "CodeFormatServer/Service/ModuleService.h"
#include "LuaParser/LuaAstVisitor.h"

class UnResolveModuleFinder : public LuaAstVisitor
{
public:
	class Scope
	{
	public:
		std::set<std::string, std::less<>> LocalNameDefineSet;
	};

	UnResolveModuleFinder()
	{
	}

	std::vector<std::shared_ptr<LuaAstNode>>& GetUndefinedModule(std::shared_ptr<LuaParser> parser)
	{
		auto ast = parser->GetAst();
		ast->AcceptChildren(*this);

		return _undefinedModule;
	}

protected:
	void VisitParamList(const std::shared_ptr<LuaAstNode>& paramList) override
	{
		for (auto param : paramList->GetChildren())
		{
			if (param->GetType() == LuaAstNodeType::Identify)
			{
				RecordLocalVariable(param);
			}
		}
	}

	void VisitLocalFunctionStatement(const std::shared_ptr<LuaAstNode>& localFunctionStatement) override
	{
		auto functionName = localFunctionStatement->FindFirstOf(LuaAstNodeType::Identify);

		if (functionName)
		{
			RecordLocalVariable(functionName);
		}
	}

	void VisitLocalStatement(const std::shared_ptr<LuaAstNode>& localStatement) override
	{
		std::shared_ptr<LuaAstNode> nameDefineList = localStatement->FindFirstOf(LuaAstNodeType::NameDefList);

		if (nameDefineList)
		{
			for (auto& nameIdentify : nameDefineList->GetChildren())
			{
				if (nameIdentify->GetType() == LuaAstNodeType::Identify)
				{
					RecordLocalVariable(nameIdentify);
				}
			}
		}
	}

	void VisitAssignment(const std::shared_ptr<LuaAstNode>& assignStatement) override
	{
		auto leftAssignStatement = assignStatement->FindFirstOf(LuaAstNodeType::ExpressionList);

		for (auto expression : leftAssignStatement->GetChildren())
		{
			if (expression->GetType() == LuaAstNodeType::Expression && !expression->GetChildren().empty())
			{
				auto expressionFirstChild = expression->GetChildren().front();
				if (expressionFirstChild->GetType() == LuaAstNodeType::PrimaryExpression)
				{
					RecordLocalVariable(expressionFirstChild);
				}
			}
		}
	}

	void VisitBlock(const std::shared_ptr<LuaAstNode>& block)
	{
		_scopeMap.insert({block, Scope()});
	}

	void VisitNameIdentify(const std::shared_ptr<LuaAstNode>& node) override
	{
		if (IsUndefinedVariable(node))
		{
			_undefinedModule.push_back(node);
		}
	}

private:
	void RecordLocalVariable(std::shared_ptr<LuaAstNode> node)
	{
		auto parent = node->GetParent();
		while (parent)
		{
			if (parent->GetType() == LuaAstNodeType::Block)
			{
				if (_scopeMap.count(parent))
				{
					auto& scope = _scopeMap[parent];
					scope.LocalNameDefineSet.insert(std::string(node->GetText()));
				}
				break;
			}
			parent = parent->GetParent();
		}
	}

	bool IsUndefinedVariable(const std::shared_ptr<LuaAstNode>& node)
	{
		auto identifyText = node->GetText();

		auto parent = node->GetParent();
		while (parent)
		{
			if (parent->GetType() == LuaAstNodeType::Block)
			{
				if (_scopeMap.count(parent))
				{
					auto& scope = _scopeMap[parent];
					auto it = scope.LocalNameDefineSet.find(identifyText);
					if (it != scope.LocalNameDefineSet.end())
					{
						return false;
					}
				}
			}
			parent = parent->GetParent();
		}

		return true;
	}

	std::shared_ptr<LuaCodeStyleOptions> _options;
	std::map<std::shared_ptr<LuaAstNode>, Scope> _scopeMap;
	std::vector<std::shared_ptr<LuaAstNode>> _undefinedModule;
};

ModuleService::ModuleService(std::shared_ptr<LanguageClient> owner)
	: Service(owner)
{
}

std::vector<vscode::Diagnostic> ModuleService::Diagnose(std::shared_ptr<LuaParser> parser,
                                                        std::shared_ptr<LuaCodeStyleOptions> options)
{
	// std::vector<vscode::Diagnostic> result;
	// UnResolveModuleFinder finder;
	// auto& undefinedModules = finder.GetUndefinedModule(parser);


	// return result;
	return {};
}

std::vector<std::string> ModuleService::GetCompleteItems()
{
	return {};
}

void ModuleService::RebuildIndexs(std::vector<std::string> files)
{
	_moduleIndex.RebuildIndex(files);
}

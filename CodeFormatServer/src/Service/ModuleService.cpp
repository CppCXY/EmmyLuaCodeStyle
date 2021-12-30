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

	void VisitBlock(const std::shared_ptr<LuaAstNode>& block) override
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

std::vector<vscode::Diagnostic> ModuleService::Diagnose(std::string_view filePath,
                                                        std::shared_ptr<LuaParser> parser,
                                                        std::shared_ptr<LuaCodeStyleOptions> options)
{
	std::vector<vscode::Diagnostic> result;
	UnResolveModuleFinder finder;
	auto& undefinedModules = finder.GetUndefinedModule(parser);
	auto& luaModules = _moduleIndex.GetModules(options);

	std::multimap<vscode::Range, ModuleIndex::Module> rangeModule;
	for (auto& undefinedModule : undefinedModules)
	{
		auto undefinedModuleName = undefinedModule->GetText();
		for (auto& luaModule : luaModules)
		{
			std::string name;

			auto dotPosition = luaModule.ModuleName.find_last_of('.');
			if (dotPosition == std::string_view::npos)
			{
				name = luaModule.ModuleName;
			}
			else
			{
				name = luaModule.ModuleName.substr(dotPosition + 1);
			}

			if (undefinedModuleName == name)
			{
				auto& diagnostic = result.emplace_back();
				diagnostic.message = "not import module";
				auto textRange = undefinedModule->GetTextRange();
				auto range = vscode::Range(
					vscode::Position(
						parser->GetLine(textRange.StartOffset),
						parser->GetColumn(textRange.StartOffset)
					),
					vscode::Position(
						parser->GetLine(textRange.EndOffset),
						parser->GetColumn(textRange.EndOffset)
					)
				);
				rangeModule.insert({range, luaModule});
				diagnostic.range = range;
				diagnostic.severity = vscode::DiagnosticSeverity::Information;
			}
		}
	}
	_diagnosticCaches[std::string(filePath)] = std::move(rangeModule);
	return result;
}

std::vector<std::string> ModuleService::GetCompleteItems()
{
	return {};
}

void ModuleService::RebuildIndexs(std::vector<std::string> files)
{
	_moduleIndex.RebuildIndex(files);
}

bool ModuleService::IsDiagnosticRange(std::string_view filePath, vscode::Range range)
{
	auto it = _diagnosticCaches.find(filePath);
	if (it == _diagnosticCaches.end())
	{
		return false;
	}

	return it->second.count(range) > 0;
}

std::vector<ModuleIndex::Module> ModuleService::GetImportModules(std::string_view filePath, vscode::Range range)
{
	auto fIt = _diagnosticCaches.find(filePath);
	if (fIt == _diagnosticCaches.end())
	{
		return {};
	}

	auto& rangeModules = fIt->second;
	auto equalRange = rangeModules.equal_range(range);
	std::vector<ModuleIndex::Module> result;
	for(auto it = equalRange.first; it != equalRange.second; ++it)
	{
		result.emplace_back(it->second);
	}

	return result;
}

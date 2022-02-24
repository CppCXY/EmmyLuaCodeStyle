#include "CodeFormatServer/Service/AstUtil/ModuleFinder.h"

ModuleFinder::ModuleFinder()
{
}

void ModuleFinder::Analysis(std::shared_ptr<LuaParser> parser)
{
	auto ast = parser->GetAst();
	ast->AcceptChildren(*this);
}


std::vector<std::shared_ptr<LuaAstNode>>& ModuleFinder::GetUndefinedModule()
{
	return _undefinedModule;
}

std::set<std::string> ModuleFinder::GetDefinedName(std::shared_ptr<LuaAstNode> untilNode)
{
	std::set<std::string> definedName;

	auto parent = untilNode->GetParent();
	while (parent)
	{
		if (parent->GetType() == LuaAstNodeType::Block)
		{
			if (_scopeMap.count(parent))
			{
				auto& scope = _scopeMap[parent];

				for(auto& name : scope.LocalNameDefineSet)
				{
					definedName.insert(name);
				}
			}
		}
		parent = parent->GetParent();
	}

	return definedName;
}


void ModuleFinder::VisitParamList(const std::shared_ptr<LuaAstNode>& paramList)
{
	for (auto param : paramList->GetChildren())
	{
		if (param->GetType() == LuaAstNodeType::Param)
		{
			RecordLocalVariable(param);
		}
	}
}

void ModuleFinder::VisitLocalFunctionStatement(const std::shared_ptr<LuaAstNode>& localFunctionStatement)
{
	auto functionName = localFunctionStatement->FindFirstOf(LuaAstNodeType::Identify);

	if (functionName)
	{
		RecordLocalVariable(functionName);
	}
}

void ModuleFinder::VisitLocalStatement(const std::shared_ptr<LuaAstNode>& localStatement)
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

void ModuleFinder::VisitAssignment(const std::shared_ptr<LuaAstNode>& assignStatement)
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

void ModuleFinder::VisitBlock(const std::shared_ptr<LuaAstNode>& block)
{
	_scopeMap.insert({ block, Scope() });
}

void ModuleFinder::VisitNameIdentify(const std::shared_ptr<LuaAstNode>& node)
{
	if (IsUndefinedVariable(node))
	{
		_undefinedModule.push_back(node);
	}
}

void ModuleFinder::RecordLocalVariable(std::shared_ptr<LuaAstNode> node)
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

bool ModuleFinder::IsUndefinedVariable(const std::shared_ptr<LuaAstNode>& node)
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

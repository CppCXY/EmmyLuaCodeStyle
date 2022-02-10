#include "CodeService/NameStyle/NameStyleChecker.h"
#include <algorithm>
#include "Util/format.h"
#include "CodeService/LanguageTranslator.h"
#include "CodeService/NameStyle/NameStyleRuleMatcher.h"

std::set<std::string, std::less<>> NameStyleChecker::TableFieldSpecialName = {
	"__add", "__sub", "__mul", "__div", "__mod", "__pow",
	"__unm", "__idiv", "__band", "__bor", "__bxor", "__bnot", "__shl",
	"__shr", "__concat", "__len", "__eq", "__lt", "__index", "__newindex",
	"__call", "__gc", "__close", "__mode", "__name"
};

std::set<std::string, std::less<>> NameStyleChecker::GlobalSpecialName = {
	"_G", "_ENV"
};

NameStyleChecker::NameStyleChecker(DiagnosisContext& ctx)
	: _ctx(ctx)
{
}

void NameStyleChecker::Analysis()
{
	for (auto checkElement : _nameStyleCheckVector)
	{
		switch (checkElement->Type)
		{
		case NameDefineType::LocalVariableName:
			{
				_ctx.GetOptions().local_name_define_style->Diagnosis(_ctx, checkElement);
				break;
			}
		case NameDefineType::ModuleDefineName:
			{
				_ctx.GetOptions().module_name_define_style->Diagnosis(_ctx, checkElement);
				break;
			}
		case NameDefineType::LocalFunctionName:
			{
				if (TableFieldSpecialName.count(checkElement->Node->GetText()))
				{
					return;
				}
				_ctx.GetOptions().local_function_name_define_style->Diagnosis(_ctx, checkElement);
				break;
			}
		case NameDefineType::GlobalVariableDefineName:
			{
				if (GlobalSpecialName.count(checkElement->Node->GetText()))
				{
					return;
				}
				_ctx.GetOptions().global_variable_name_define_style->Diagnosis(_ctx, checkElement);
				break;
			}
		case NameDefineType::ParamName:
			{
				_ctx.GetOptions().function_param_name_style->Diagnosis(_ctx, checkElement);
				break;
			}
		case NameDefineType::ImportModuleName:
			{
				_ctx.GetOptions().require_module_name_style->Diagnosis(_ctx, checkElement);
				break;
			}
		case NameDefineType::ClassVariableName:
			{
				_ctx.GetOptions().class_name_define_style->Diagnosis(_ctx, checkElement);
				break;
			}
		case NameDefineType::FunctionDefineName:
			{
				if (TableFieldSpecialName.count(checkElement->Node->GetText()))
				{
					return;
				}

				_ctx.GetOptions().function_name_define_style->Diagnosis(_ctx, checkElement);
				break;
			}
		case NameDefineType::TableFieldDefineName:
			{
				// 忽略元方法
				if (TableFieldSpecialName.count(checkElement->Node->GetText()))
				{
					return;
				}
				_ctx.GetOptions().table_field_name_define_style->Diagnosis(_ctx, checkElement);
				break;
			}
		}
	}
}

void NameStyleChecker::VisitLocalStatement(const std::shared_ptr<LuaAstNode>& localStatement)
{
	std::shared_ptr<LuaAstNode> nameDefineList = nullptr;
	std::shared_ptr<LuaAstNode> expressionList = nullptr;

	for (auto child : localStatement->GetChildren())
	{
		if (child->GetType() == LuaAstNodeType::NameDefList)
		{
			nameDefineList = child;
		}
		else if (child->GetType() == LuaAstNodeType::ExpressionList)
		{
			expressionList = child;
			break;
		}
	}

	if (nameDefineList && expressionList)
	{
		std::vector<std::shared_ptr<LuaAstNode>> nameVector;
		for (auto& nameIdentify : nameDefineList->GetChildren())
		{
			if (nameIdentify->GetType() == LuaAstNodeType::Identify)
			{
				nameVector.push_back(nameIdentify);
			}
		}

		std::vector<std::shared_ptr<LuaAstNode>> expressionVector;

		for (auto& expression : expressionList->GetChildren())
		{
			if (expression->GetType() == LuaAstNodeType::Expression)
			{
				expressionVector.push_back(expression);
			}
		}

		for (std::size_t index = 0; index < nameVector.size(); index++)
		{
			auto nameIdentify = nameVector[index];
			if (index >= expressionVector.size())
			{
				auto checkElement = std::make_shared<CheckElement>(NameDefineType::LocalVariableName, nameIdentify);
				_nameStyleCheckVector.push_back(checkElement);
				RecordLocalVariable(checkElement);
				continue;
			}
			auto expression = expressionVector[index];
			auto callExpression = expression->FindFirstOf(LuaAstNodeType::CallExpression);

			if (callExpression)
			{
				// 将约定做成规范
				auto methodNameNode = callExpression->FindFirstOf(LuaAstNodeType::PrimaryExpression);
				if (methodNameNode)
				{
					if (methodNameNode->GetText() == "require" || methodNameNode->GetText() == "import")
					{
						auto callArgsList = callExpression->FindFirstOf(LuaAstNodeType::CallArgList);

						auto checkElement = std::make_shared<CheckElement>(
							NameDefineType::ImportModuleName, nameIdentify, callArgsList);
						_nameStyleCheckVector.push_back(checkElement);
						RecordLocalVariable(checkElement);
						continue;
					}
					else if (methodNameNode->GetText() == "Class" || methodNameNode->GetText() == "class")
					{
						auto callArgsList = callExpression->FindFirstOf(LuaAstNodeType::CallArgList);
						auto checkElement = std::make_shared<CheckElement>(
							NameDefineType::ClassVariableName, nameIdentify, callArgsList);
						_nameStyleCheckVector.push_back(checkElement);
						RecordLocalVariable(checkElement);
						continue;
					}
				}
			}
			auto checkElement = std::make_shared<CheckElement>(NameDefineType::LocalVariableName, nameIdentify);
			_nameStyleCheckVector.push_back(checkElement);
			RecordLocalVariable(checkElement);
		}
	}
	else if (nameDefineList)
	{
		for (auto& nameIdentify : nameDefineList->GetChildren())
		{
			if (nameIdentify->GetType() == LuaAstNodeType::Identify)
			{
				auto checkElement = std::make_shared<CheckElement>(NameDefineType::LocalVariableName, nameIdentify);
				_nameStyleCheckVector.push_back(checkElement);
				RecordLocalVariable(checkElement);
			}
		}
	}
}

void NameStyleChecker::VisitParamList(const std::shared_ptr<LuaAstNode>& paramList)
{
	for (auto param : paramList->GetChildren())
	{
		if (param->GetType() == LuaAstNodeType::Identify)
		{
			_nameStyleCheckVector.push_back(std::make_shared<CheckElement>(NameDefineType::ParamName, param));
		}
	}
}

void NameStyleChecker::VisitLocalFunctionStatement(const std::shared_ptr<LuaAstNode>& localFunctionStatement)
{
	for (auto child : localFunctionStatement->GetChildren())
	{
		if (child->GetType() == LuaAstNodeType::Identify)
		{
			_nameStyleCheckVector.push_back(std::make_shared<CheckElement>(NameDefineType::LocalFunctionName, child));
			break;
		}
	}
}

void NameStyleChecker::VisitFunctionStatement(const std::shared_ptr<LuaAstNode>& functionStatement)
{
	for (auto child : functionStatement->GetChildren())
	{
		if (child->GetType() == LuaAstNodeType::NameExpression && !child->GetChildren().empty())
		{
			std::shared_ptr<LuaAstNode> lastElement = child->GetChildren().back();
			while (lastElement)
			{
				if (lastElement->GetType() == LuaAstNodeType::IndexExpression && !lastElement->GetChildren().
					empty())
				{
					lastElement = lastElement->GetChildren().back();
				}
				else
				{
					break;
				}
			}

			if (lastElement && lastElement->GetType() == LuaAstNodeType::Identify
				|| lastElement->GetType() == LuaAstNodeType::PrimaryExpression)
			{
				_nameStyleCheckVector.push_back(
					std::make_shared<CheckElement>(NameDefineType::FunctionDefineName, lastElement));
			}
		}
	}
}

void NameStyleChecker::VisitAssignment(const std::shared_ptr<LuaAstNode>& assignStatement)
{
	auto leftAssignStatement = assignStatement->FindFirstOf(LuaAstNodeType::ExpressionList);

	for (auto expression : leftAssignStatement->GetChildren())
	{
		if (expression->GetType() == LuaAstNodeType::Expression && !expression->GetChildren().empty())
		{
			auto expressionFirstChild = expression->GetChildren().front();
			if (expressionFirstChild->GetType() == LuaAstNodeType::PrimaryExpression)
			{
				if (IsGlobal(expressionFirstChild))
				{
					_nameStyleCheckVector.push_back(
						std::make_shared<CheckElement>(NameDefineType::GlobalVariableDefineName, expressionFirstChild));
				}
			}
			else if (expressionFirstChild->GetType() == LuaAstNodeType::IndexExpression)
			{
				std::shared_ptr<LuaAstNode> lastElement = expressionFirstChild->GetChildren().back();
				while (lastElement)
				{
					if (lastElement->GetType() == LuaAstNodeType::IndexExpression && !lastElement->GetChildren().
						empty())
					{
						lastElement = lastElement->GetChildren().back();
					}
					else
					{
						break;
					}
				}

				if (lastElement && lastElement->GetType() == LuaAstNodeType::Identify)
				{
					_nameStyleCheckVector.push_back(
						std::make_shared<CheckElement>(NameDefineType::TableFieldDefineName, lastElement));
				}
			}
		}
	}
}

void NameStyleChecker::VisitTableField(const std::shared_ptr<LuaAstNode>& tableField)
{
	auto identify = tableField->FindFirstOf(LuaAstNodeType::Identify);

	if (identify)
	{
		_nameStyleCheckVector.push_back(std::make_shared<CheckElement>(NameDefineType::TableFieldDefineName, identify));
	}
}

void NameStyleChecker::VisitReturnStatement(const std::shared_ptr<LuaAstNode>& returnStatement)
{
	if (InTopBlock(returnStatement))
	{
		auto expressionList = returnStatement->FindFirstOf(LuaAstNodeType::ExpressionList);
		if (expressionList && !expressionList->GetChildren().empty())
		{
			auto firstReturnExpression = expressionList->GetChildren().front();
			if (firstReturnExpression->GetType() == LuaAstNodeType::Expression)
			{
				auto primaryReturnIdentify = firstReturnExpression->FindFirstOf(LuaAstNodeType::PrimaryExpression);
				if (primaryReturnIdentify)
				{
					auto block = returnStatement->GetParent();

					if (_scopeMap.count(block))
					{
						auto& scope = _scopeMap[block];
						auto it = scope.LocalVariableMap.find(primaryReturnIdentify->GetText());
						if (it != scope.LocalVariableMap.end())
						{
							it->second->Type = NameDefineType::ModuleDefineName;
						}
					}
				}
			}
		}
	}
}

void NameStyleChecker::VisitBlock(const std::shared_ptr<LuaAstNode>& block)
{
	_scopeMap.insert({block, Scope()});
}

void NameStyleChecker::RecordLocalVariable(std::shared_ptr<CheckElement> checkElement)
{
	auto node = checkElement->Node;
	auto parent = node->GetParent();
	while (parent)
	{
		if (parent->GetType() == LuaAstNodeType::Block)
		{
			if (_scopeMap.count(parent))
			{
				auto& scope = _scopeMap[parent];
				scope.LocalVariableMap.insert({std::string(node->GetText()), checkElement});
			}
			break;
		}
		parent = parent->GetParent();
	}
}

bool NameStyleChecker::InTopBlock(std::shared_ptr<LuaAstNode> chunkBlockStatement)
{
	auto parent = chunkBlockStatement->GetParent();
	if (parent && parent->GetType() == LuaAstNodeType::Block)
	{
		auto chunkParent = parent->GetParent();
		return chunkParent && chunkParent->GetType() == LuaAstNodeType::Chunk;
	}
	return false;
}

bool NameStyleChecker::IsGlobal(std::shared_ptr<LuaAstNode> node)
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
				auto it = scope.LocalVariableMap.find(identifyText);
				if (it != scope.LocalVariableMap.end())
				{
					return false;
				}
			}
		}
		parent = parent->GetParent();
	}

	return true;
}

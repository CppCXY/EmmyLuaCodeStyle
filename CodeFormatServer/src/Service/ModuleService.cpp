#include "CodeFormatServer/Service/ModuleService.h"
#include "CodeFormatServer/Service/AstUtil/ModuleFinder.h"
#include "LuaParser/LuaAstVisitor.h"
#include "Util/StringUtil.h"
#include "Util/format.h"
#include "Util/Url.h"

ModuleService::ModuleService(std::shared_ptr<LanguageClient> owner)
	: Service(owner)
{
}

std::vector<vscode::Diagnostic> ModuleService::Diagnose(std::string_view filePath,
                                                        std::shared_ptr<LuaParser> parser)
{
	std::vector<vscode::Diagnostic> result;
	ModuleFinder finder;
	finder.Analysis(parser);

	auto& undefinedModules = finder.GetUndefinedModule();
	auto luaModules = _moduleIndex.GetModules(filePath);

	std::multimap<vscode::Range, LuaModule> rangeModule;
	for (auto& undefinedModule : undefinedModules)
	{
		auto undefinedModuleName = undefinedModule->GetText();
		for (auto& luaModule : luaModules)
		{
			if (StringUtil::IsStringEqualIgnoreCase(undefinedModuleName, luaModule->MatchName))
			{
				auto& diagnostic = result.emplace_back();
				diagnostic.message = "need import module";
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

				rangeModule.insert({
					range, LuaModule{
						.ModuleName = luaModule->ModuleName,
						.FilePath = luaModule->FilePath,
						.Name = std::string(undefinedModuleName)
					}
				});

				diagnostic.range = range;
				diagnostic.severity = vscode::DiagnosticSeverity::Hint;
			}
		}
	}
	_diagnosticCaches[std::string(filePath)] = std::move(rangeModule);
	return result;
}

bool ModuleService::IsDiagnosticRange(std::string_view filePath, vscode::Range range)
{
	auto it = _diagnosticCaches.find(filePath);
	if (it == _diagnosticCaches.end())
	{
		return false;
	}

	if (range.start == range.end)
	{
		for (auto& pair : it->second)
		{
			if (pair.first.start <= range.start && pair.first.end >= range.end)
			{
				return true;
			}
		}
		return false;
	}
	else
	{
		return it->second.count(range) > 0;
	}
}

std::vector<ModuleService::LuaModule> ModuleService::GetImportModules(std::string_view filePath, vscode::Range range)
{
	auto fIt = _diagnosticCaches.find(filePath);
	if (fIt == _diagnosticCaches.end())
	{
		return {};
	}

	std::vector<LuaModule> result;
	auto& rangeModules = fIt->second;

	if (range.start == range.end)
	{
		for (auto& pair : rangeModules)
		{
			if (pair.first.start <= range.start && pair.first.end >= range.end)
			{
				result.push_back(pair.second);
			}
		}
	}
	else
	{
		auto itPair = rangeModules.equal_range(range);

		for (auto it = itPair.first; it != itPair.second; it++)
		{
			result.push_back(it->second);
		}
	}
	return result;
}

vscode::Range ModuleService::FindRequireRange(std::shared_ptr<LuaParser> parser)
{
	vscode::Range range;
	auto chunkAst = parser->GetAst();
	if (chunkAst->GetChildren().empty())
	{
		return range;
	}

	auto blockNode = chunkAst->GetChildren().front();

	std::shared_ptr<LuaAstNode> lastNode = nullptr;

	auto& children = blockNode->GetChildren();
	for (auto statement : children)
	{
		auto type = statement->GetType();

		switch (type)
		{
		case LuaAstNodeType::Comment:
			{
				break;
			}
		case LuaAstNodeType::LocalStatement:
			{
				std::shared_ptr<LuaAstNode> expressionList = statement->FindFirstOf(LuaAstNodeType::ExpressionList);
				if (expressionList)
				{
					auto expression = expressionList->FindFirstOf(LuaAstNodeType::Expression);
					if (expression)
					{
						auto callExpression = expression->FindFirstOf(LuaAstNodeType::CallExpression);
						if (callExpression)
						{
							// 将约定做成规范
							auto methodNameNode = callExpression->FindFirstOf(LuaAstNodeType::PrimaryExpression);
							if (methodNameNode)
							{
								if (methodNameNode->GetText() == "require")
								{
									break;
								}
							}
						}
					}
				}
				goto endLoop;
			}
		default:
			{
				goto endLoop;
			}
		}
		lastNode = statement;
	}
endLoop:
	if (lastNode)
	{
		vscode::Position insertPosition(parser->GetLine(lastNode->GetTextRange().EndOffset) + 1, 0);
		range.start = insertPosition;
		range.end = insertPosition;
	}

	return range;
}

ModuleIndex& ModuleService::GetIndex()
{
	return _moduleIndex;
}

std::vector<vscode::CompletionItem> ModuleService::GetModuleCompletions(std::shared_ptr<LuaAstNode> expression,
                                                                        std::shared_ptr<LuaParser> parser,
                                                                        std::shared_ptr<LuaCodeStyleOptions> options,
                                                                        std::string_view uri)
{
	auto path = url::UrlToFilePath(uri);
	std::vector<vscode::CompletionItem> result;
	ModuleFinder finder;
	finder.Analysis(parser);

	auto definedNames = finder.GetDefinedName(expression);

	auto luaModules = _moduleIndex.GetModules(path);
	auto insertRange = FindRequireRange(parser);


	for (auto& luaModule : luaModules)
	{
		auto& completion = result.emplace_back();

		std::string& name = luaModule->MatchName;
		if(definedNames.count(name) > 0)
		{
			continue;
		}

		completion.label = name;
		completion.detail = format("({})", luaModule->ModuleName);
		completion.documentation = format("import from {}", luaModule->FilePath);
		completion.insertText = name;
		completion.kind = vscode::CompletionItemKind::Module;
		// vscode::CompletionItemLabelDetails labelDetail;
		// labelDetail.detail = completion.detail;
		// labelDetail.description = completion.detail;
		// completion.labelDetails = labelDetail;

		vscode::Command command;
		command.title = completion.detail;
		command.command = "emmylua.import.me";
		command.arguments.push_back(uri);
		command.arguments.push_back(insertRange.Serialize());
		auto object = nlohmann::json::object();
		object["moduleName"] = luaModule->ModuleName;
		object["path"] = luaModule->FilePath;
		object["name"] = name;
		command.arguments.push_back(object);
		completion.command = command;
	}

	return result;
}

#include "CodeFormatServer/Service/CompletionService.h"
#include <functional>
#include "CodeFormatServer/Service/ModuleService.h"
#include "Util/format.h"

using namespace std::placeholders;

CompletionService::CompletionService(std::shared_ptr<LanguageClient> owner)
	: Service(owner)
{
}

void CompletionService::Start()
{
	AddCompleteContributor({LuaAstNodeType::NameIdentify},
	                       std::bind(&ModuleService::GetModuleCompletions, GetService<ModuleService>().get(),
	                                 _1, _2, _3, _4));
	// AddCompleteContributor({LuaAstNodeType::LiteralExpression},
	//                        std::bind(&ModuleService::GetRequireCompletions, GetService<ModuleService>().get(),
	//                                  _1, _2, _3, _4)
	// );
}

std::vector<vscode::CompletionItem> CompletionService::GetCompletions(std::string_view uri,
                                                                      vscode::Position position,
                                                                      std::shared_ptr<LuaParser> parser,
                                                                      std::shared_ptr<LuaCodeStyleOptions> options)
{
	auto expression = FindAstFromPosition(position, parser);

	if (!expression)
	{
		return {};
	}

	std::vector<vscode::CompletionItem> result;

	for (auto& pair : _contributors)
	{
		if (Match(expression, pair.first))
		{
			auto completions = pair.second(expression, parser, options, uri);
			std::copy(completions.begin(), completions.end(), std::back_inserter(result));
		}
	}

	return result;
}

void CompletionService::AddCompleteContributor(std::vector<LuaAstNodeType> pattern, CompleteContributor contributor)
{
	_contributors.emplace_back(pattern, contributor);
}

std::shared_ptr<LuaAstNode> CompletionService::FindAstFromPosition(vscode::Position position,
                                                                   std::shared_ptr<LuaParser> parser)
{
	auto ast = parser->GetAst();
	auto luaFile = parser->GetLuaFile();
	auto offset = luaFile->GetOffsetFromPosition(position.line, position.character > 0 ? position.character - 1 : 0);

	auto textOffset = TextRange(offset, offset);
	while (true)
	{
		if (ast->GetChildren().empty())
		{
			return ast;
		}
		bool hasNext = false;

		for (auto child : ast->GetChildren())
		{
			if (child->GetTextRange().Contain(textOffset))
			{
				hasNext = true;
				ast = child;
				break;
			}
		}

		if (!hasNext)
		{
			return ast;
		}
	}

	return nullptr;
}

bool CompletionService::Match(std::shared_ptr<LuaAstNode> node, std::vector<LuaAstNodeType>& pattern)
{
	for (auto type : pattern)
	{
		if (node == nullptr || node->GetType() != type)
		{
			return false;
		}

		node = node->GetParent();
	}

	return true;
}

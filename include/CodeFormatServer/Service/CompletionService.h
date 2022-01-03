#pragma once

#include "Service.h"
#include <memory>

class CompletionService : public Service
{
public:
	LANGUAGE_SERVICE(CompletionService);

	using CompleteContributor = std::function<std::vector<vscode::CompletionItem>(
		std::shared_ptr<LuaAstNode>, std::shared_ptr<LuaParser>, std::shared_ptr<LuaCodeStyleOptions>,
		std::string_view)>;

	CompletionService(std::shared_ptr<LanguageClient> owner);

	void Start() override;

	std::vector<vscode::CompletionItem> GetCompletions(std::string_view uri,
	                                                   vscode::Position position,
	                                                   std::shared_ptr<LuaParser> parser,
	                                                   std::shared_ptr<LuaCodeStyleOptions> options);

	void AddCompleteContributor(std::vector<LuaAstNodeType> pattern, CompleteContributor contributor);

private:
	static std::shared_ptr<LuaAstNode> FindAstFromPosition(vscode::Position position,
	                                                       std::shared_ptr<LuaParser> parser);

	bool Match(std::shared_ptr<LuaAstNode> node, std::vector<LuaAstNodeType>& pattern);

	std::vector<std::pair<std::vector<LuaAstNodeType>, CompleteContributor>> _contributors;
};

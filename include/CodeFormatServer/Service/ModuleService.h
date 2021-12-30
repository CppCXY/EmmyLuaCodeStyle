#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include "LuaParser/LuaParser.h"
#include "CodeFormatServer/VSCode.h"
#include "CodeService/LuaCodeStyleOptions.h"
#include "Service.h"
#include "Indexs/ModuleIndex.h"


class ModuleService : public Service
{
public:
	LANGUAGE_SERVICE(ModuleService);

	explicit ModuleService(std::shared_ptr<LanguageClient> owner);

	std::vector<vscode::Diagnostic> Diagnose(std::string_view filePath, std::shared_ptr<LuaParser> parser,
	                                         std::shared_ptr<LuaCodeStyleOptions> options);
	std::vector<std::string> GetCompleteItems();

	void RebuildIndexs(std::vector<std::string> files);
	bool IsDiagnosticRange(std::string_view filePath, vscode::Range range);
	std::vector<ModuleIndex::Module> GetImportModules(std::string_view filePath, vscode::Range range);

	vscode::Range FindRequireRange(std::shared_ptr<LuaParser> parser);

private:
	ModuleIndex _moduleIndex;
	std::map<std::string, std::multimap<vscode::Range, ModuleIndex::Module>, std::less<>> _diagnosticCaches;
};

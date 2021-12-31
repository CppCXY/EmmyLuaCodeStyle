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

	class LuaModule
	{
	public:
		std::string ModuleName;
		std::string FilePath;
		std::string Name;
	};

	explicit ModuleService(std::shared_ptr<LanguageClient> owner);

	std::vector<vscode::Diagnostic> Diagnose(std::string_view filePath, std::shared_ptr<LuaParser> parser,
	                                         std::shared_ptr<LuaCodeStyleOptions> options);
	std::vector<std::string> GetCompleteItems();

	bool IsDiagnosticRange(std::string_view filePath, vscode::Range range);
	std::vector<LuaModule> GetImportModules(std::string_view filePath, vscode::Range range);

	vscode::Range FindRequireRange(std::shared_ptr<LuaParser> parser);

	ModuleIndex& GetIndex();
private:
	ModuleIndex _moduleIndex;
	std::map<std::string, std::multimap<vscode::Range, LuaModule>, std::less<>> _diagnosticCaches;
};

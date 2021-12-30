#pragma once

#include <vector>
#include <memory>
#include <string>
#include "LuaParser/LuaParser.h"
#include "CodeFormatServer/VSCode.h"
#include "CodeService/LuaCodeStyleOptions.h"
#include "Service.h"
#include "Indexs/ModuleIndex.h"

class ModuleService: public Service
{
public:
	LANGUAGE_SERVICE(ModuleService);

	explicit ModuleService(std::shared_ptr<LanguageClient> owner);

	std::vector<vscode::Diagnostic> Diagnose(std::shared_ptr<LuaParser> parser, std::shared_ptr<LuaCodeStyleOptions> options);
	std::vector<std::string> GetCompleteItems();

	void RebuildIndexs(std::vector<std::string> files);

private:
	ModuleIndex _moduleIndex;
};
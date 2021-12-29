#pragma once

#include "LuaParser/LuaParser.h"
#include "CodeFormatServer/VSCode.h"
#include "CodeService/LuaCodeStyleOptions.h"

class ModuleService
{
public:
	std::vector<vscode::Diagnostic> GetModuleDiagnostics(std::shared_ptr<LuaParser> parser, std::shared_ptr<LuaCodeStyleOptions> options);
	std::vector<std::string> GetCompleteItems();

};
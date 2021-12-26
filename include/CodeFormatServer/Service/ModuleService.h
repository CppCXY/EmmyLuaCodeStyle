#pragma once

#include "LuaParser/LuaParser.h"
#include "CodeFormatServer/VSCode.h"

class ModuleService
{
public:
	std::vector<vscode::Diagnostic> GetModuleDiagnostics();
	std::vector<std::string> GetCompleteItems();

};
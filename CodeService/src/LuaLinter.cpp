#include "CodeService/LuaLinter.h"

LuaLinter::LuaLinter(std::shared_ptr<LuaParser> luaParser, LuaLinterOptions& options)
	: _luaParser(luaParser),
	  _options(options)
{
}

void LuaLinter::BuildLintElement()
{
}

std::vector<LuaDiagnosisInfo> LuaLinter::GetLintResult()
{
}

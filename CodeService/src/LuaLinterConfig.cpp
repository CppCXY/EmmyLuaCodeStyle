#include "CodeService/LuaLinterConfig.h"

std::shared_ptr<LuaLinterConfig> LuaLinterConfig::LoadFromFile(const std::string& path)
{
	return nullptr;
}

LuaLinterConfig::LuaLinterConfig(std::string&& source)
	: _source(source),
	  _linterOptions(nullptr)
{
}

void LuaLinterConfig::Parse()
{
	_linterOptions = std::make_shared<LuaLinterOptions>();
}

std::shared_ptr<LuaLinterOptions> LuaLinterConfig::Generate(std::string_view fileUri)
{
}

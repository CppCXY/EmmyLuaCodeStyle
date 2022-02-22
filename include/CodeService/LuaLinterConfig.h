#pragma once

#include <memory>
#include <string>

#include "LuaLinterOptions.h"

class LuaLinterConfig
{
public:
	static std::shared_ptr<LuaLinterConfig> LoadFromFile(const std::string& path);

	LuaLinterConfig(std::string&& source);

	void Parse();

	std::shared_ptr<LuaLinterOptions> Generate(std::string_view fileUri);

private:
	std::string _source;
	std::shared_ptr<LuaLinterOptions> _linterOptions;
};
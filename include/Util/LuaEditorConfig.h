#pragma once

#include <map>
#include <string>
#include <memory>

class LuaEditorConfig
{
public:
	static std::shared_ptr<LuaEditorConfig> LoadFromFile(const std::string& path);

	LuaEditorConfig(std::string&& source);

	void Parse();

	bool Exist(std::string_view key);

	std::string Get(std::string_view key);

private:
	std::string _source;
	std::map<std::string, std::string, std::less<>> _configMap;
};

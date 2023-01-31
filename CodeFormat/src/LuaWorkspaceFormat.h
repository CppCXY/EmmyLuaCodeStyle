#pragma once

#include <string>
#include <string_view>
#include <filesystem>
#include "LuaFormat.h"
#include "CodeService/Config/LuaStyle.h"
#include "CodeService/Config/LuaEditorConfig.h"

class LuaWorkspaceFormat
{
public:
	LuaWorkspaceFormat(std::string_view workspace);

	bool SetConfigPath(std::string_view config);

	void AddIgnoresByFile(std::string_view gitIgnoreFile);

	void AddIgnores(std::string_view pattern);

	void SetAutoDetectConfig(bool detect);

	void SetKeyValues(std::map<std::string, std::string, std::less<>>& keyValues);

	void ReformatWorkspace();

	bool CheckWorkspace();
	
private:
	void CollectEditorconfig();
	LuaStyle GetStyle(std::string_view path);

	std::filesystem::path _workspace;
	bool _autoDetectConfig;
	std::vector<std::pair<std::string, std::shared_ptr<LuaEditorConfig>>> _editorConfigVector;
	LuaStyle _defaultStyle;
	std::vector<std::string> _ignorePattern;
};

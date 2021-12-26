#pragma once

#include <map>
#include <string>
#include <memory>
#include "CodeService/LuaCodeStyleOptions.h"

class LuaEditorConfig
{
public:
	class Section
	{
	public:
		std::map<std::string, std::string, std::less<>> ConfigMap;
	};

	static std::shared_ptr<LuaEditorConfig> LoadFromFile(const std::string& path);

	LuaEditorConfig(std::string&& source);

	void Parse();

	std::shared_ptr<LuaCodeStyleOptions> Generate(std::string_view fileUri);

	void SetWorkspace(std::string_view workspace);

	void SetRootWorkspace(std::string_view rootWorkspace);
private:
	void ParseFromSection(std::shared_ptr<LuaCodeStyleOptions> options,
	                      std::map<std::string, std::string, std::less<>>& configMap);

	std::string _workspace;
	std::string _rootWorkspace;
	std::string _source;
	std::map<std::string, std::shared_ptr<Section>, std::less<>> _sectionMap;
	std::map<std::string, std::shared_ptr<LuaCodeStyleOptions>> _optionPatternMap;
};

#pragma once

#include <map>
#include <vector>
#include <string>
#include <set>
#include <memory>
#include "CodeFormatServer/Service/Indexs/Config/ModuleConfig.h"

class ModuleIndex
{
public:
	class Module
	{
	public:
		Module(std::string_view moduleName = "", std::string_view filePath = "", std::string_view matchName = "")
			: ModuleName(moduleName),
			  FilePath(filePath),
			  MatchName(matchName)
		{
		}

		std::string ModuleName;
		std::string FilePath;
		std::string MatchName;
	};

	ModuleIndex();

	void BuildModule(std::string workspaceUri, std::string_view moduleConfigPath);

	void RemoveModule(std::string workspaceUri);

	void SetDefaultModule(std::string workspaceUri);

	void BuildIndex(const std::vector<std::string>& files);

	void ClearFile(std::string_view filePath);

	void RebuildIndex(const std::vector<std::string>& files);

	std::vector<std::shared_ptr<Module>> GetModules(std::string_view filePath);

	std::shared_ptr<ModuleConfig> GetConfig(std::string_view filePath);
private:
	std::map<std::string, std::vector<std::shared_ptr<Module>>> _moduleIndex;
	std::map<std::string, std::shared_ptr<ModuleConfig>> _moduleConfigMap;
};

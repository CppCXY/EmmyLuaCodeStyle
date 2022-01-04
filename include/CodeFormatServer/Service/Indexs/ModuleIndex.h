#pragma once

#include <map>
#include <vector>
#include <string>
#include <set>
#include <memory>
#include "CodeFormatServer/Service/Indexs/Config/ModuleConfig.h"
#include "Module.h"

class ModuleIndex
{
public:
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

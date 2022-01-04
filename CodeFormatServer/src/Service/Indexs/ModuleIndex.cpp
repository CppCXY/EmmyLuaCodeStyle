#include "CodeFormatServer/Service/Indexs/ModuleIndex.h"
#include "CodeFormatServer/LanguageClient.h"
#include <filesystem>
#include "Util/Url.h"

namespace fs = std::filesystem;

ModuleIndex::ModuleIndex()
{
}

void ModuleIndex::BuildModule(std::string workspaceUri, std::string_view moduleConfigPath)
{
	auto workspacePath = url::UrlToFilePath(workspaceUri);
	auto moduleConfig = ModuleConfig::LoadFromFile(moduleConfigPath);
	moduleConfig->module_root = workspacePath;
	_moduleConfigMap[workspacePath] = moduleConfig;
}

void ModuleIndex::RemoveModule(std::string workspaceUri)
{
	auto workspacePath = url::UrlToFilePath(workspaceUri);
	auto it = _moduleConfigMap.find(workspacePath);
	if (it != _moduleConfigMap.end())
	{
		_moduleConfigMap.erase(it);
	}
}

void ModuleIndex::SetDefaultModule(std::string workspaceUri)
{
	if (!_moduleConfigMap.empty())
	{
		return;
	}

	auto workspacePath = url::UrlToFilePath(workspaceUri);
	auto moduleConfig = std::make_shared<ModuleConfig>();
	moduleConfig->import.push_back("root");
	moduleConfig->import_function = "require";
	moduleConfig->module_root = workspacePath;
	moduleConfig->name = "root";
	moduleConfig->separator = '.';
	moduleConfig->_exportRules.push_back(ExportRule::init_to_dir_module);
	_moduleConfigMap[workspacePath] = moduleConfig;
}

void ModuleIndex::BuildIndex(const std::vector<std::string>& files)
{
	for (auto& filePath : files)
	{
		auto config = GetConfig(filePath);

		if (!config)
		{
			continue;
		}

		auto relativePath = std::filesystem::relative(filePath, config->module_root);
		std::string modulePath = relativePath.replace_extension("").string();

		modulePath = config->GetModuleName(modulePath);

		std::string matchName;

		auto dotPosition = modulePath.find_last_of(config->separator);
		if (dotPosition == std::string_view::npos)
		{
			matchName = modulePath;
		}
		else
		{
			matchName = modulePath.substr(dotPosition + 1);
		}

		if (config->special_module.count(modulePath) > 0)
		{
			matchName = config->special_module.at(modulePath).Name;
		}

		// 一些代码喜欢路径带连字符
		for (auto& c : matchName)
		{
			if (c == '-')
			{
				c = '_';
			}
		}

		if (_moduleIndex.count(config->name) == 0)
		{
			_moduleIndex.insert({config->name, std::vector<std::shared_ptr<Module>>()});
		}

		_moduleIndex.at(config->name).push_back(std::make_shared<Module>(modulePath, filePath, matchName));
	}
}

void ModuleIndex::ClearFile(std::string_view filePath)
{
	auto config = GetConfig(filePath);
	if (!config)
	{
		return;
	}

	if (_moduleIndex.count(config->name) > 0)
	{
		auto& vec = _moduleIndex.at(config->name);
		for (auto it = vec.begin(); it != vec.end(); ++it)
		{
			if ((*it)->FilePath == filePath)
			{
				vec.erase(it);
				return;
			}
		}
	}
}

void ModuleIndex::RebuildIndex(const std::vector<std::string>& files)
{
	_moduleIndex.clear();
	BuildIndex(files);
}

std::vector<std::shared_ptr<Module>> ModuleIndex::GetModules(std::string_view filePath)
{
	std::vector<std::shared_ptr<Module>> result;

	auto config = GetConfig(filePath);
	if (!config)
	{
		return result;
	}
	for (auto& from : config->import)
	{
		auto it = _moduleIndex.find(from);

		if (it != _moduleIndex.end())
		{
			std::copy(it->second.begin(), it->second.end(), std::back_inserter(result));
		}
	}
	return result;
}

std::shared_ptr<ModuleConfig> ModuleIndex::GetConfig(std::string_view filePath)
{
	std::shared_ptr<ModuleConfig> config = nullptr;
	std::size_t matchLength = 0;
	for (auto& pair : _moduleConfigMap)
	{
		auto relativePath = std::filesystem::relative(filePath, pair.second->module_root).string();
		if (!relativePath.starts_with("."))
		{
			if (pair.second->module_root.size() > matchLength)
			{
				matchLength = relativePath.size();
				config = pair.second;
			}
		}
	}

	return config;
}

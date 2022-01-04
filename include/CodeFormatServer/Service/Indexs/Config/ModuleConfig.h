#pragma once
#include <map>
#include <string>
#include <vector>
#include <memory>
#include "ExportRule.h"

class ModuleConfig
{
public:
	class SpecialModule
	{
	public:
		SpecialModule(std::string_view name, std::string_view module)
			: Name(name), Module(module)
		{
		}

		std::string Name;
		std::string Module;
	};

	static std::shared_ptr<ModuleConfig> LoadFromFile(std::string_view filePath);

	ModuleConfig();

	std::string GetModuleName(std::string_view path);

	// 配置项采用 snake_case 命名

	std::string name;
	std::string module_root;
	std::vector<std::string> import;
	char separator;
	std::string import_function;
	std::map<std::string, SpecialModule> special_module;
	std::vector<ExportRule> _exportRules;
};

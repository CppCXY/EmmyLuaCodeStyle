#include "CodeFormatServer/Service/Indexs/Config/ModuleConfig.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>

std::shared_ptr<ModuleConfig> ModuleConfig::LoadFromFile(std::string_view filePath)
{
	auto moduleConfig = std::make_shared<ModuleConfig>();

	std::fstream fin(std::string(filePath), std::ios::in | std::ios::binary);

	if (!fin.is_open())
	{
		return moduleConfig;
	}

	std::stringstream s;
	s << fin.rdbuf();
	auto text = s.str();
	try
	{
		auto json = nlohmann::json::parse(text);
		if (json["name"].is_string())
		{
			moduleConfig->name = json["name"];
		}
		else
		{
			moduleConfig->name = "root";
		}

		if (json["import"].is_array())
		{
			for (auto module : json["import"])
			{
				if (module.is_string())
				{
					moduleConfig->import.push_back(module);
				}
			}
		}

		if (moduleConfig->import.empty())
		{
			moduleConfig->import.push_back("root");
		}

		if (json["separator"].is_string())
		{
			std::string separator = json["separator"];
			if (separator.size() == 1)
			{
				moduleConfig->separator = separator[0];
			}
			else
			{
				moduleConfig->separator = '.';
			}
		}

		if (json["import.function"].is_string())
		{
			moduleConfig->import_function = json["import.function"];
		}

		if (json["special.module"].is_array())
		{
			for (auto specialModule : json["special.module"])
			{
				if (specialModule.is_object())
				{
					if (specialModule["name"].is_string() && specialModule["module"].is_string())
					{
						std::string name = specialModule["name"];
						std::string module = specialModule["module"];
						moduleConfig->special_module.insert({module, SpecialModule(name, module)});
					}
				}
			}
		}

		if(json["export.rule"].is_array())
		{
			for (auto rule : json["export.rule"])
			{
				if (rule.is_string())
				{
					std::string strRule = rule;
					if(strRule == "init_to_dir_module")
					{
						moduleConfig->_exportRules.push_back(ExportRule::init_to_dir_module);
					}
					else if(strRule == "only_filename")
					{
						moduleConfig->_exportRules.push_back(ExportRule::only_filename);
					}
				}
			}
		}
		else
		{
			moduleConfig->_exportRules.push_back(ExportRule::init_to_dir_module);
		}

	}
	catch (nlohmann::json::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return moduleConfig;
}

ModuleConfig::ModuleConfig()
	: separator('.'),
	  import_function("require")
{
}

std::string ModuleConfig::GetModuleName(std::string_view path)
{
	std::string moduleName(path);
	for (auto& c : moduleName)
	{
		if (c == '/' || c == '\\')
		{
			c = separator;
		}
	}

	for (auto rule : _exportRules)
	{
		switch (rule)
		{
		case ExportRule::init_to_dir_module:
			{
				// .init 特殊规则
				if (moduleName.ends_with(".init"))
				{
					return moduleName.substr(0, moduleName.size() - 5);
				}
				break;
			}
		case ExportRule::only_filename:
			{
				auto dotPosition = moduleName.find_last_of(separator);
				if (dotPosition == std::string::npos)
				{
					return moduleName;
				}
				else
				{
					return moduleName.substr(dotPosition + 1);
				}
				break;
			}
		default:
			{
				break;
			}
		}
	}

	return moduleName;
}

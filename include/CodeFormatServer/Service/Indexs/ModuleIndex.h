#pragma once

#include <map>
#include <vector>
#include <string>
#include <set>
#include <memory>
#include "CodeService/LuaCodeStyleOptions.h"

class ModuleIndex
{
public:
	class Module
	{
	public:
		Module(const std::string& moduleName, const std::string& filePath)
			: ModuleName(moduleName),
			  FilePath(filePath)
		{
		}

		std::string ModuleName;
		std::string FilePath;
	};

	ModuleIndex();

	void BuildIndex(const std::vector<std::string>& files);

	void ClearFile(std::string_view filePath);

	void RebuildIndex(const std::vector<std::string>& files);

	std::vector<Module>& GetModules(std::shared_ptr<LuaCodeStyleOptions> options);

private:
	std::map<std::string, std::vector<Module>> _moduleIndex;
	std::vector<Module> _emptyModules;
};

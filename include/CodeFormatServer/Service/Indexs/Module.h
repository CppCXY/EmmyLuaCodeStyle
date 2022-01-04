#pragma once

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
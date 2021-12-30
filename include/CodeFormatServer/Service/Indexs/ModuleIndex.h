#pragma once

#include <map>
#include <vector>
#include <string>
#include <set>

class ModuleIndex
{
public:
	ModuleIndex();

	void BuildIndex(const std::vector<std::string>& files);

	void RebuildIndex(const std::vector<std::string>& files);

	std::vector<std::string> GetModules(std::string filename);

private:
	std::map<std::string, std::set<std::string>> _moduleIndex;
};

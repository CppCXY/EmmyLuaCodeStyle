#include "CodeFormatServer/Indexs/ModuleIndex.h"
#include "CodeFormatServer/LanguageClient.h"


ModuleIndex::ModuleIndex()
{
}

void ModuleIndex::BuildIndex(const std::vector<std::string>& files)
{
	for (auto& filename : files)
	{
		auto options = LanguageClient::GetInstance().GetOptions(filename);
		auto relativePath = std::filesystem::relative(filename, options->export_root);
		std::string modulePath = relativePath.replace_extension("").string();
		if (modulePath.starts_with("."))
		{
			continue;
		}

		for (auto& c : modulePath)
		{
			if (c == '/' || c == '\\')
			{
				c = '.';
			}
		}

		if (modulePath.ends_with(".init"))
		{
			modulePath = modulePath.substr(0, modulePath.size() - 5);
		}

		if (_moduleIndex.count(options->export_root) == 0)
		{
			_moduleIndex.insert({options->export_root, std::set<std::string>()});
		}

		_moduleIndex.at(options->export_root).insert(modulePath);
	}
}

void ModuleIndex::ReBuildIndex(const std::vector<std::string>& files)
{
	_moduleIndex.clear();
	BuildIndex(files);
}

std::vector<std::string> ModuleIndex::GetModules(std::string filename)
{
	auto options = LanguageClient::GetInstance().GetOptions(filename);

	std::vector<std::string> modules;
	for (auto& from : options->import_from)
	{
		auto it = _moduleIndex.find(from);

		if (it != _moduleIndex.end())
		{
			for (auto& modulePath : it->second)
			{
				modules.emplace_back(modulePath);
			}
		}
	}
	return modules;
}

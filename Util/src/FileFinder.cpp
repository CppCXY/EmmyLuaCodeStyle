#include "Util/FileFinder.h"

FileFinder::FileFinder(std::filesystem::path root)
	:_root(root)
{
}

void FileFinder::AddIgnoreDirectory(const std::string& extension)
{
	_ignoreDirectory.insert(extension);
}

void FileFinder::AddFindExtension(const std::string& extension)
{
	_findExtension.insert(extension);
}

std::vector<std::string> FileFinder::FindFiles()
{
	std::vector<std::string> files;

	CollectFile(_root, files);

	return files;
}

void FileFinder::CollectFile(std::filesystem::path directoryPath, std::vector<std::string>& paths)
{
	if (!std::filesystem::exists(directoryPath))
	{
		return;
	}

	for (auto& it : std::filesystem::directory_iterator(directoryPath))
	{
		if (std::filesystem::is_directory(it.status()))
		{
			auto filename = it.path().filename().string();
			if (_ignoreDirectory.count(filename) == 0) {
				CollectFile(it.path(), paths);
			}
		}
		else if(std::filesystem::is_regular_file(it.status()))
		{
			std::string extension = it.path().extension().string();
			if(_findExtension.count(extension) == 1)
			{
				paths.push_back(it.path().string());
			}
		}
	}
}

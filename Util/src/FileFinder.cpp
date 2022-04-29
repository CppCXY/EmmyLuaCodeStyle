#include "Util/FileFinder.h"
#include "wildcards/match.hpp"
#include "Util/StringUtil.h"

FileFinder::FileFinder(std::filesystem::path root)
	: _root(root)
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

void FileFinder::AddFindFile(const std::string& fileName)
{
	_findFile.insert(fileName);
}

void FileFinder::AddignorePatterns(const std::string& pattern)
{
	_ignorePatterns.push_back(pattern);
}

std::vector<std::string> FileFinder::FindFiles()
{
	std::vector<std::string> files;

	CollectFile(_root, files);

	if(_ignorePatterns.empty())
	{
		return files;
	}

	std::vector<std::string> resultFiles;
	auto rootPath = _root.string();
	for(auto& originFile: files)
	{
		if (StringUtil::StartWith(originFile, rootPath))
		{
			auto matchFilePath = originFile.substr(rootPath.size());

			for (auto& pattern : _ignorePatterns) {
				auto f = wildcards::match(matchFilePath, pattern);

				if(f.res)
				{
					goto nextFile;
				}
			}

		}
		resultFiles.push_back(originFile);
nextFile:
		void();//ignore
	}
	return resultFiles;
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
			if (_ignoreDirectory.count(filename) == 0)
			{
				CollectFile(it.path(), paths);
			}
		}
		else if (std::filesystem::is_regular_file(it.status()))
		{
			if (!_findFile.empty())
			{
				auto filename = it.path().filename().string();
				if (_findFile.count(filename) == 1)
				{
					paths.push_back(it.path().string());
				}
			}
			if (!_findExtension.empty())
			{
				std::string extension = it.path().extension().string();
				if (_findExtension.count(extension) == 1)
				{
					paths.push_back(it.path().string());
				}
			}
		}
	}
}

#pragma once

#include <string>
#include <string_view>
#include <filesystem>
#include <set>
#include <vector>

class FileFinder {
public:
    FileFinder(std::filesystem::path root);

    void AddIgnoreDirectory(const std::string &extension);

    void AddFindExtension(const std::string &extension);

    void AddFindFile(const std::string &fileName);

    void AddignorePatterns(const std::string &pattern);

    std::vector<std::string> FindFiles();

private:
    void CollectFile(std::filesystem::path directoryPath, std::vector<std::string> &paths);

    std::filesystem::path _root;
    std::set<std::string> _ignoreDirectory;
    std::set<std::string> _findExtension;
    std::set<std::string> _findFile;
    std::vector<std::string> _ignorePatterns;
};

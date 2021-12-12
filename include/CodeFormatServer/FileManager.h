// #pragma once
//
// #include <map>
// #include <string>
// #include <string_view>
//
// #include "LuaParser/LuaParser.h"
// #include "VSCode.h"
//
// /*
//  * 该优化被取消，还不如全量更新快呢
//  */
// class FileManager
// {
// public:
// 	FileManager();
// 	~FileManager();
//
// 	/*
// 	 * 增量更新
// 	 */
// 	void UpdateFile(std::string_view uri, vscode::Range range, std::string& text);
//
// 	/*
// 	 * 全量更新
// 	 */
// 	void UpdateFile(std::string_view uri, std::string& text);
//
// 	void ReleaseFile(std::string_view uri);
//
// 	std::string GetFileText(std::string_view uri);
// private:
//
// 	std::vector<std::string> GetTextLines(std::string& text);
//
// 	std::map<std::string, std::vector<std::string>, std::less<>> _fileMap;
// };
//

// #include "CodeFormatServer/FileManager.h"
// #include <algorithm>
// #include "Util/Utf8.h"
//
// FileManager::FileManager()
// {
// }
//
// FileManager::~FileManager()
// {
// }
//
// void FileManager::UpdateFile(std::string_view uri, vscode::Range range, std::string& text)
// {
// 	auto it = _fileMap.find(uri);
// 	if (it == _fileMap.end())
// 	{
// 		return UpdateFile(uri, text);
// 	}
//
// 	auto textLines = GetTextLines(text);
//
// 	auto& file = it->second;
//
// 	std::vector<std::string> newFile;
//
// 	auto startLine = range.start.line;
// 	auto endLine = range.end.line;
// 	auto startCharacter = range.start.character;
// 	auto endCharacter = range.end.character;
//
// 	for (std::size_t lineNumber = 0; lineNumber < startLine; lineNumber++)
// 	{
// 		if (lineNumber < file.size())
// 		{
// 			newFile.emplace_back(std::move(file[lineNumber]));
// 		}
// 		else
// 		{
// 			newFile.emplace_back("\n");
// 		}
// 	}
//
// 	if (startCharacter != 0 && startLine < file.size() && !textLines.empty())
// 	{
// 		auto byteNumber = utf8::Utf8nByteNum(file[startLine].data(), file[startLine].size(), startCharacter + 1);
// 		textLines.front() = file[startLine].substr(0, byteNumber) + textLines.front();
// 	}
//
// 	if (endLine < file.size() && !textLines.empty())
// 	{
// 		auto byteNumber = utf8::Utf8nByteNum(file[endLine].data(), file[endLine].size(), endCharacter + 1);
//
// 		textLines.back().append(file[endLine].substr(byteNumber));
// 	}
//
// 	for (auto& line : textLines)
// 	{
// 		newFile.emplace_back(std::move(line));
// 	}
//
//
// 	for (std::size_t lineNumber = endLine + 1; lineNumber < file.size(); lineNumber++)
// 	{
// 		newFile.emplace_back(std::move(file[lineNumber]));
// 	}
//
// 	file = newFile;
// }
//
// void FileManager::UpdateFile(std::string_view uri, std::string& text)
// {
// 	_fileMap[std::string(uri)] = GetTextLines(text);
// }
//
// void FileManager::ReleaseFile(std::string_view uri)
// {
// 	auto it = _fileMap.find(uri);
//
// 	if (it != _fileMap.end())
// 	{
// 		_fileMap.erase(it);
// 	}
// }
//
// std::string FileManager::GetFileText(std::string_view uri)
// {
// 	std::string fileText;
//
// 	auto it = _fileMap.find(uri);
// 	if (it == _fileMap.end())
// 	{
// 		return "";
// 	}
//
// 	std::size_t fileSize = std::accumulate(it->second.begin(), it->second.end(), 0,
// 	                                       [](auto x, const std::string& line)
// 	                                       {
// 		                                       return x + line.size();
// 	                                       });
//
// 	fileText.reserve(fileSize);
//
// 	for (auto& line : it->second)
// 	{
// 		fileText.append(line);
// 	}
// 	return fileText;
// }
//
//
// std::vector<std::string> FileManager::GetTextLines(std::string& text)
// {
// 	std::vector<std::string> textLines;
//
// 	std::string_view source = text;
// 	std::size_t sepLen = 1;
//
// 	while (true)
// 	{
// 		const std::size_t sepIndex = source.find_first_of('\n', 0);
// 		if (sepIndex == std::string_view::npos)
// 		{
// 			textLines.emplace_back(source);
//
// 			break;
// 		}
// 		else
// 		{
// 			textLines.emplace_back(source.substr(0, sepIndex + sepLen));
// 			source = source.substr(sepIndex + sepLen);
// 		}
// 	}
// 	return textLines;
// }

#pragma once

#include <string_view>
#include <string>
#include <vector>

class LuaFile
{
public:
	LuaFile(std::string_view filename, std::string&& fileText);

	int GetLine(int offset);

	int GetColumn(int offset);

	int GetTotalLine();

	bool IsEmptyLine(int line);

	void PushLine(int offset);

	std::string_view GetSource();

	void SetTotalLine(int line);

	void SetFilename(std::string_view filename);

	std::string_view GetFilename() const;
protected:
	std::string _source;
	std::string _filename;

	int _linenumber;
	std::vector<int> _lineOffsetVec;
};
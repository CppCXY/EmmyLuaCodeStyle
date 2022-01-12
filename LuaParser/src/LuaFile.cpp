#include "LuaParser/LuaFile.h"

#include "Util/Utf8.h"

LuaFile::LuaFile(std::string_view filename, std::string&& fileText)
	: _source(fileText),
	  _filename(filename),
	  _linenumber(0)
{
	_lineOffsetVec.push_back(0);
}


int LuaFile::GetLine(int offset)
{
	if (_lineOffsetVec.empty())
	{
		return 0;
	}

	int maxLine = static_cast<int>(_lineOffsetVec.size()) - 1;
	int targetLine = maxLine;
	int upperLine = maxLine;
	int lowestLine = 0;

	while (true)
	{
		if (_lineOffsetVec[targetLine] > offset)
		{
			upperLine = targetLine;

			targetLine = (upperLine + lowestLine) / 2;

			if (targetLine == 0)
			{
				return targetLine;
			}
		}
		else
		{
			if (upperLine - targetLine <= 1)
			{
				return targetLine;
			}

			lowestLine = targetLine;

			targetLine = (upperLine + lowestLine) / 2;
		}
	}

	return 0;
}

int LuaFile::GetColumn(int offset)
{
	int line = GetLine(offset);

	int lineStartOffset = _lineOffsetVec[line];

	int bytesLength = offset - lineStartOffset;

	return static_cast<int>(utf8::Utf8nLen(_source.data() + lineStartOffset, static_cast<std::size_t>(bytesLength)));
}

int LuaFile::GetOffsetFromPosition(int line, int character)
{
	if (line >= _lineOffsetVec.size() || line < 0)
	{
		return _source.size() + 1;
	}

	int lineStartOffset = _lineOffsetVec[line];
	int nextOffset = 0;
	if (line + 1 >= _lineOffsetVec.size())
	{
		nextOffset = _source.size();
	}
	else
	{
		nextOffset = _lineOffsetVec[line + 1];
	}

	std::size_t offset = utf8::Utf8nByteNum(_source.data() + lineStartOffset, nextOffset - lineStartOffset, character);
	return lineStartOffset + static_cast<int>(offset); 
}

int LuaFile::GetTotalLine()
{
	return _linenumber;
}

bool LuaFile::IsEmptyLine(int line)
{
	if (line < 0 || line >= _lineOffsetVec.size())
	{
		return true;
	}
	int lineStartOffset = _lineOffsetVec[line];
	int nextLineStartOffset = 0;

	if (line == _linenumber)
	{
		nextLineStartOffset = static_cast<int>(_source.size());
	}
	else
	{
		nextLineStartOffset = _lineOffsetVec[line + 1];
	}

	for (int offset = lineStartOffset; offset < nextLineStartOffset; offset++)
	{
		char ch = _source[offset];
		if (ch != '\n' && ch != '\r' && ch != '\t' && ch != ' ')
		{
			return false;
		}
	}

	return true;
}


void LuaFile::PushLine(int offset)
{
	_lineOffsetVec.push_back(offset);
}

std::string_view LuaFile::GetSource()
{
	return _source;
}

void LuaFile::SetTotalLine(int line)
{
	_linenumber = line;
}

void LuaFile::SetFilename(std::string_view filename)
{
	_filename = filename;
}

std::string_view LuaFile::GetFilename() const
{
	return _filename;
}

#include "LuaParser/LuaFile.h"

#include "Util/Utf8.h"

LuaFile::LuaFile(std::string_view filename, std::string&& fileText)
	: _source(fileText),
	  _filename(filename),
	  _linenumber(0),
	  _lineState(EndOfLine::UNKNOWN)
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
	if (line < 0)
	{
		return _source.size() + 1;
	}

	std::size_t sizeLine = line;

	if (sizeLine >= _lineOffsetVec.size())
	{
		return _source.size() + 1;
	}

	int lineStartOffset = _lineOffsetVec[line];
	std::size_t nextOffset = 0;
	if (sizeLine + 1 >= _lineOffsetVec.size())
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
	if (line < 0 || line >= static_cast<int>(_lineOffsetVec.size()))
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

bool LuaFile::OnlyEmptyCharBefore(int offset)
{
	auto source = GetSource();
	auto line = GetLine(offset);
	auto start = GetOffsetFromPosition(line, 0);

	for (; start < offset; start++)
	{
		auto ch = source[start];
		if (ch != '\t' && ch != ' ')
		{
			return false;
		}
	}

	return true;
}

std::string& LuaFile::GetSource()
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

void LuaFile::UpdateLineInfo(int startLine)
{
	int totalLine = _lineOffsetVec.size() - 1;
	std::size_t startOffset = 0;
	if (totalLine < startLine)
	{
		startLine = 0;
	}
	else
	{
		startOffset = _lineOffsetVec[startLine];
	}

	_lineOffsetVec.resize(startLine + 1);

	for (; startOffset < _source.size(); startOffset ++)
	{
		if (_source[startOffset] == '\n')
		{
			_lineOffsetVec.push_back(startOffset + 1);
		}
	}
}

void LuaFile::Reset()
{
	_lineOffsetVec.resize(0);
	_lineOffsetVec.push_back(0);
	_linenumber = 0;
	_lineState = EndOfLine::UNKNOWN;
}

void LuaFile::SetEndOfLineState(EndOfLine endOfLine)
{
	switch (_lineState)
	{
	case EndOfLine::UNKNOWN:
		{
			_lineState = endOfLine;
			break;
		}
	case EndOfLine::CR:
	case EndOfLine::LF:
	case EndOfLine::CRLF:
		{
			if (_lineState != endOfLine)
			{
				_lineState = EndOfLine::MIX;
			}
			break;
		}
	default:
		{
			break;
		}
	}
}

EndOfLine LuaFile::GetEndOfLine() const
{
	return _lineState;
}

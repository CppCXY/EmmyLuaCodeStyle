#pragma once

#include <string_view>
#include <string>
#include <vector>
#include "LuaParser/Types/EndOfLineType.h"

class LuaFile
{
public:
	explicit LuaFile(std::string&& fileText);

	std::size_t GetLine(std::size_t offset) const;

	std::size_t GetColumn(std::size_t offset) const;

    std::size_t GetLineOffset(std::size_t offset) const;

	std::size_t GetOffset(std::size_t line, std::size_t character) const;

	std::size_t GetTotalLine() const;

	void PushLine(std::size_t offset);

	std::string_view GetSource() const;

    std::string_view Slice(std::size_t startOffset, std::size_t endOffset) const;

	void SetTotalLine(std::size_t line);

	void UpdateLineInfo(std::size_t startLine = 0);

	void Reset();

	void SetEndOfLineState(EndOfLine endOfLine);

	EndOfLine GetEndOfLine() const;

	std::size_t GetLineRestCharacter(std::size_t offset);

	std::string_view GetIndentString(std::size_t offset) const;

    bool IsEmptyLine(std::size_t line) const;
protected:
	std::string _source;

	std::size_t _linenumber;
	std::vector<std::size_t> _lineOffsetVec;
	EndOfLine _lineState;
};
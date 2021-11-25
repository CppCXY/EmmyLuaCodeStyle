#pragma once

class LuaFormatRange
{
public:
	LuaFormatRange(int startLine, int endLine)
		: StartLine(startLine),
		  EndLine(endLine),
		  StartCharacter(0),
		  EndCharacter(0)
	{
	}

	int StartLine;
	int EndLine;

	int StartCharacter;
	int EndCharacter;
};

#pragma once

class LuaFormatRange
{
public:
	LuaFormatRange(int startLine, int endLine)
		: StartLine(startLine),
		  EndLine(endLine)
	{
	}

	int StartLine;
	int EndLine;
};

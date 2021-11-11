#pragma once

#include <string>
#include "LuaParser/TextRange.h"

class LuaDiagnosisPosition
{
public:
	int Line = 0;
	int Character = 0;
};

class LuaDiagnosisRange
{
public:
	LuaDiagnosisPosition Start;
	LuaDiagnosisPosition End;
};

class LuaDiagnosisInfo
{
public:
	std::string Message;
	LuaDiagnosisRange Range;
};
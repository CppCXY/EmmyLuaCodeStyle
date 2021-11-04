#pragma once

#include <string>
#include "LuaParser/TextRange.h"

class LuaDiagnosisInfo
{
public:
	std::string Message;
	TextRange Range;
};
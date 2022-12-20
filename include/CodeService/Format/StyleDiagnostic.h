#pragma once

#include <string>
#include "DiagnosticType.h"

class StyleDiagnostic
{
public:
	std::string Message;
	TextRange Range;
	DiagnosticType Type;
	std::string Data;
};

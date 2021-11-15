#pragma once

#include <string_view>
#include <map>

#include "CodeService/LuaDiagnosisInfo.h"
#include "CodeService/DiagnosisElement/LuaDiagnosisType.h"

#define DECLARE_DIAGNOSIS(DiagnosisClass)\
	virtual LuaDiagnosisType GetType() { return LuaDiagnosisType::DiagnosisClass; }

class DiagnosisElement
{
public:
	DECLARE_DIAGNOSIS(DiagnosisElement)

	DiagnosisElement(LuaDiagnosisRange range);
	virtual ~DiagnosisElement();
	virtual std::string_view GetMessage(const std::map<std::string, std::string>& translateMap);
	LuaDiagnosisRange GetRange();

protected:
	LuaDiagnosisRange _range;
};
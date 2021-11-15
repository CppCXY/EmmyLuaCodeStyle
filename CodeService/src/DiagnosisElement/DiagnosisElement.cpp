#include "CodeService/DiagnosisElement/DiagnosisElement.h"

DiagnosisElement::DiagnosisElement(LuaDiagnosisRange range)
	: _range(range)
{
}

DiagnosisElement::~DiagnosisElement()
{
}

std::string_view DiagnosisElement::GetMessage(const std::map<std::string, std::string>& translateMap)
{
	return "";
}

LuaDiagnosisRange DiagnosisElement::GetRange()
{
	return _range;
}

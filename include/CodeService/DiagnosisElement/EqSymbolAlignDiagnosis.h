#pragma once

#include "DiagnosisElement.h"

class EqSymbolAlignDiagnosis : public DiagnosisElement
{
public:
	DECLARE_DIAGNOSIS(EqSymbolAlignDiagnosis)

	EqSymbolAlignDiagnosis(LuaDiagnosisRange range, int shouldAlignToCharacter);

	std::string_view GetMessage(const std::map<std::string, std::string>& translateMap) override;
private:
	int _shouldAlignToCharacter;
};

#include "CodeService/DiagnosisElement/EqSymbolAlignDiagnosis.h"
#include "Util/format.h"
EqSymbolAlignDiagnosis::EqSymbolAlignDiagnosis(LuaDiagnosisRange range, int shouldAlignToCharacter)
	: DiagnosisElement(range),
	  _shouldAlignToCharacter(shouldAlignToCharacter)
{
}

std::string_view EqSymbolAlignDiagnosis::GetMessage(const std::map<std::string, std::string>& translateMap)
{
	return format(T.Get("'=' should align to character {}", _shouldAlignToCharacter));
}

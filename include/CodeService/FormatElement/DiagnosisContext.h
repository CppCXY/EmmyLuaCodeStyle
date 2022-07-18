#pragma once
#include <memory>
#include <stack>
#include "FormatContext.h"
#include "CodeService/Diagnosis/LuaDiagnosisInfo.h"
#include "CodeService/LuaCodeStyleOptions.h"
#include "LuaParser/LuaParser.h"

class DiagnosisContext: public FormatContext
{
public:
	DiagnosisContext(std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options);

	void PushDiagnosis(std::string_view message, TextRange range, DiagnosisType type, std::string data = "");

	void PushDiagnosis(std::string_view message, LuaDiagnosisPosition start, LuaDiagnosisPosition end, DiagnosisType type, std::string data = "");

	void SetCharacterCount(int character);

	void SetLineMaxLength(int line, int character);

	void DiagnoseLine();

	std::vector<LuaDiagnosisInfo> GetDiagnosisInfos();
private:
	std::vector<LuaDiagnosisInfo> _diagnosisInfos;
	std::map<int, int> _lineMaxLengthMap;
};

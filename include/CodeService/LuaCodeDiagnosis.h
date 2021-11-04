#pragma once

#include <memory>
#include "LuaDiagnosisOptions.h"
#include "LuaParser/TextRange.h"
#include "LuaParser/LuaParser.h"
#include "LuaDiagnosisInfo.h"
#include "DiagnosisElement/DiagnosisElement.h"

class LuaCodeDiagnosis
{
public:
	LuaCodeDiagnosis(std::shared_ptr<LuaParser> luaParser, LuaDiagnosisOptions& options);

	void BuildDiagnosisElement();

	std::vector<LuaDiagnosisInfo> GetDiagnosisInfos();

protected:
	std::shared_ptr<DiagnosisElement> DiagnosisBlock(std::shared_ptr<LuaAstNode> block);


private:
	std::shared_ptr<DiagnosisElement> _env;
	std::shared_ptr<LuaParser> _parser;
	LuaDiagnosisOptions _options;

};

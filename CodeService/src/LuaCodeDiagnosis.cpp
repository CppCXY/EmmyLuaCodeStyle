#include "CodeService/LuaCodeDiagnosis.h"
#include "CodeService/DiagnosisElement/DiagnosisIndent.h"

LuaCodeDiagnosis::LuaCodeDiagnosis(std::shared_ptr<LuaParser> luaParser, LuaDiagnosisOptions& options)
	: _env(nullptr),
	  _parser(luaParser),
	  _options(options)
{
}

void LuaCodeDiagnosis::BuildDiagnosisElement()
{
	auto chunkNode = _parser->GetAst();

	_env = DiagnosisBlock(chunkNode->GetChildren()[0]);
}

std::vector<LuaDiagnosisInfo> LuaCodeDiagnosis::GetDiagnosisInfos()
{
	return {};
}

std::shared_ptr<DiagnosisElement> LuaCodeDiagnosis::DiagnosisBlock(std::shared_ptr<LuaAstNode> block)
{
	auto env = std::make_shared<DiagnosisElement>();




	return nullptr;
}

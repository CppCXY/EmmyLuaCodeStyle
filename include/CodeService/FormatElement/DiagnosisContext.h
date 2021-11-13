#pragma once
#include <memory>
#include <stack>

#include "CodeService/LuaDiagnosisInfo.h"
#include "CodeService/LuaCodeStyleOptions.h"
#include "LuaParser/LuaParser.h"

class DiagnosisContext
{
public:
	DiagnosisContext(std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options);

	void AddIndent(int specialIndent = -1);

	void RecoverIndent();

	void PushDiagnosis(std::string_view message, TextRange range);

	void PushDiagnosis(std::string_view message, LuaDiagnosisPosition start, LuaDiagnosisPosition end);

	int GetLine(int offset);

	int GetColumn(int offset);

	std::size_t GetCharacterCount() const;

	void SetCharacterCount(int character);

	void SetLineMaxLength(int line, int character);

	std::size_t GetCurrentIndent() const;

	std::vector<LuaDiagnosisInfo> GetDiagnosisInfos();

	const LuaCodeStyleOptions& GetOptions();

	std::shared_ptr<LuaParser> GetParser();

private:
	std::stack<int, std::vector<int>> _indentStack;
	LuaCodeStyleOptions& _options;

	std::size_t _characterCount;
	std::shared_ptr<LuaParser> _parser;
	std::vector<LuaDiagnosisInfo> _diagnosisInfos;
	std::map<int, int> _lineMaxLengthMap;
};

#include "CodeService/FormatElement/DiagnosisContext.h"
#include "CodeService/LanguageTranslator.h"
#include "Util/format.h"

DiagnosisContext::DiagnosisContext(std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options)
	: FormatContext(parser, options)
{
}


void DiagnosisContext::PushDiagnosis(std::string_view message, TextRange range, DiagnosisType type, std::string data)
{
	LuaDiagnosisPosition start(GetLine(range.StartOffset), GetColumn(range.StartOffset));
	LuaDiagnosisPosition end(GetLine(range.EndOffset), GetColumn(range.EndOffset) + 1);
	PushDiagnosis(message, start, end, type, data);
}

void DiagnosisContext::PushDiagnosis(std::string_view message, LuaDiagnosisPosition start, LuaDiagnosisPosition end,
                                     DiagnosisType type, std::string data)
{
	_diagnosisInfos.push_back(LuaDiagnosisInfo{std::string(message), LuaDiagnosisRange(start, end), type, data});
}

void DiagnosisContext::SetCharacterCount(int character)
{
	_characterCount = character;
}

void DiagnosisContext::SetLineMaxLength(int line, int character)
{
	_lineMaxLengthMap[line] = character;
}

std::vector<LuaDiagnosisInfo> DiagnosisContext::GetDiagnosisInfos()
{
	if (!_lineMaxLengthMap.empty())
	{
		for (auto& [line, character] : _lineMaxLengthMap)
		{
			LuaDiagnosisPosition start(line, _options.max_line_length);
			LuaDiagnosisPosition end(line, character);
			PushDiagnosis(Util::format(LText("The line width should not exceed {}"), _options.max_line_length), start, end,
			              DiagnosisType::MaxLineWidth);
		}
		_lineMaxLengthMap.clear();
	}

	if (_options.insert_final_newline && !_parser->IsEmptyLine(_parser->GetTotalLine()))
	{
		LuaDiagnosisPosition start(_parser->GetTotalLine(), _parser->GetColumn(
			                           static_cast<int>(_parser->GetSource().size())));
		LuaDiagnosisPosition end(_parser->GetTotalLine() + 1, 0);
		PushDiagnosis(LText("The code must end with a new line"), start, end, DiagnosisType::EndWithNewLine);
	}
	return _diagnosisInfos;
}

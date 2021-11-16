#include "CodeService/FormatElement/DiagnosisContext.h"

#include "CodeService/LanguageTranslator.h"
#include "Util/format.h"

DiagnosisContext::DiagnosisContext(std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options)
	: _parser(parser),
	  _options(options)
{
}

void DiagnosisContext::AddIndent(int specialIndent)
{
	if (!_options.use_tab)
	{
		int newIndent = 0;
		if (specialIndent == -1)
		{
			if (_indentStack.empty())
			{
				_indentStack.push(0);
				return;
			}

			auto& topIndent = _indentStack.top();
			newIndent = _options.indent + topIndent;
		}
		else
		{
			newIndent = specialIndent;
		}
		_indentStack.push(newIndent);
	}
	else //这个算法可能会有问题
	{
		int newIndent = 0;
		if (specialIndent == -1)
		{
			if (_indentStack.empty())
			{
				_indentStack.push(0);
				return;
			}

			auto& topIndent = _indentStack.top();
			// 一次只增一个\t
			newIndent = 1 + topIndent;
		}
		else
		{
			// 我会认为存在一个换算
			// 当你制定了一个缩进，则我会认为保底有一个缩进
			newIndent = std::max(1, specialIndent / 8);
		}
		_indentStack.push(newIndent);
	}
}

void DiagnosisContext::RecoverIndent()
{
	_indentStack.pop();
}


void DiagnosisContext::PushDiagnosis(std::string_view message, TextRange range)
{
	LuaDiagnosisPosition start(GetLine(range.StartOffset), GetColumn(range.StartOffset));
	LuaDiagnosisPosition end(GetLine(range.EndOffset), GetColumn(range.EndOffset));
	PushDiagnosis(message, start, end);
}

void DiagnosisContext::PushDiagnosis(std::string_view message, LuaDiagnosisPosition start, LuaDiagnosisPosition end)
{
	_diagnosisInfos.push_back(LuaDiagnosisInfo{std::string(message), LuaDiagnosisRange(start, end)});
}

int DiagnosisContext::GetLine(int offset)
{
	return _parser->GetLine(offset);
}

int DiagnosisContext::GetColumn(int offset)
{
	return _parser->GetColumn(offset);
}

std::size_t DiagnosisContext::GetCharacterCount() const
{
	return _characterCount;
}

void DiagnosisContext::SetCharacterCount(int character)
{
	_characterCount = character;
}

void DiagnosisContext::SetLineMaxLength(int line, int character)
{
	_lineMaxLengthMap[line] = character;
}

std::size_t DiagnosisContext::GetCurrentIndent() const
{
	return _indentStack.top();
}

std::vector<LuaDiagnosisInfo> DiagnosisContext::GetDiagnosisInfos()
{
	if (!_lineMaxLengthMap.empty())
	{
		for (auto& [line, character] : _lineMaxLengthMap)
		{
			LuaDiagnosisPosition start(line, _options.max_line_length);
			LuaDiagnosisPosition end(line, character);
			PushDiagnosis(format(T("The line width should not exceed {}"), _options.max_line_length), start, end);
		}
		_lineMaxLengthMap.clear();
	}
	return _diagnosisInfos;
}

const LuaCodeStyleOptions& DiagnosisContext::GetOptions()
{
	return _options;
}

std::shared_ptr<LuaParser> DiagnosisContext::GetParser()
{
	return _parser;
}

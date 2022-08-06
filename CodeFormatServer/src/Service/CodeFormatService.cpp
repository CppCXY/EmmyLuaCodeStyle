#include "CodeFormatServer/Service/CodeFormatService.h"

#include "CodeFormatServer/Service/CodeActionService.h"
#include "CodeFormatServer/Service/CommandService.h"
#include "CodeService/LuaFormatter.h"
#include "CodeService/FormatElement/DiagnosisContext.h"
#include "CodeService/NameStyle/NameStyleChecker.h"

CodeFormatService::CodeFormatService(std::shared_ptr<LanguageClient> owner)
	: Service(owner),
	  _spellChecker(std::make_shared<CodeSpellChecker>())
{
}

std::vector<vscode::Diagnostic> CodeFormatService::Diagnose(std::string_view filePath,
                                                            std::shared_ptr<LuaParser> parser,
                                                            std::shared_ptr<LuaCodeStyleOptions> options)
{
	auto& settings = _owner->GetSettings();
	std::vector<vscode::Diagnostic> diagnostics;

	LuaFormatter formatter(parser, *options);
	formatter.BuildFormattedElement();
	DiagnosisContext ctx(parser, *options);
	formatter.CalculateDiagnosisInfos(ctx);

	if (settings.lintCodeStyle)
	{
		NameStyleChecker styleChecker(ctx);
		styleChecker.Analysis();
	}

	if (settings.spellEnable)
	{
		_spellChecker->Analysis(ctx, _customDictionary);
	}

	ctx.DiagnoseLine();

	auto diagnosisInfos = ctx.GetDiagnosisInfos();
	for (auto diagnosisInfo : diagnosisInfos)
	{
		auto& diagnosis = diagnostics.emplace_back();
		diagnosis.message = diagnosisInfo.Message;
		diagnosis.range = vscode::Range(
			vscode::Position(
				diagnosisInfo.Range.Start.Line,
				diagnosisInfo.Range.Start.Character
			),
			vscode::Position(
				diagnosisInfo.Range.End.Line,
				diagnosisInfo.Range.End.Character
			));
		diagnosis.severity = vscode::DiagnosticSeverity::Warning;
		switch (diagnosisInfo.type)
		{
		case DiagnosisType::Indent:
		case DiagnosisType::Blank:
		case DiagnosisType::Align:
			{
				diagnosis.code = GetService<CodeActionService>()->GetCode(CodeActionService::DiagnosticCode::Reformat);
				break;
			}
		case DiagnosisType::Spell:
			{
				diagnosis.severity = vscode::DiagnosticSeverity::Information;
				diagnosis.code = GetService<CodeActionService>()->GetCode(CodeActionService::DiagnosticCode::Spell);
				diagnosis.data = diagnosisInfo.Data;
				break;
			}
		default:
			{
				break;
			}
		}
	}

	return diagnostics;
}

std::string CodeFormatService::Format(std::shared_ptr<LuaParser> parser, std::shared_ptr<LuaCodeStyleOptions> options)
{
	LuaFormatter formatter(parser, *options);
	formatter.BuildFormattedElement();

	return formatter.GetFormattedText();
}

std::string CodeFormatService::RangeFormat(LuaFormatRange& range,
                                           std::shared_ptr<LuaParser> parser,
                                           LuaCodeStyleOptions& options)
{
	LuaFormatter formatter(parser, options);
	formatter.BuildFormattedElement();
	return formatter.GetRangeFormattedText(range);
}

void CodeFormatService::LoadDictionary(std::string_view path)
{
	if (_owner->GetSettings().spellEnable)
	{
		_spellChecker->LoadDictionary(path);
	}
}

void CodeFormatService::SetCustomDictionary(std::vector<std::string>& dictionary)
{
	_customDictionary.clear();

	for (auto& word : dictionary)
	{
		_customDictionary.insert(word);
	}
}

std::shared_ptr<CodeSpellChecker> CodeFormatService::GetSpellChecker()
{
	return _spellChecker;
}

#include "FormatService.h"

//#include "CodeActionService.h"
//#include "CommandService.h"
//#include "CodeService/LuaFormatter.h"
//#include "CodeService/FormatElement/DiagnosisContext.h"
//#include "CodeService/NameStyle/NameStyleChecker.h"
#include "LanguageServer.h"
#include "CodeService/Format/FormatBuilder.h"


FormatService::FormatService(LanguageServer *owner)
        : Service(owner)
//	  _spellChecker(std::make_shared<CodeSpellChecker>())
{
}
//
//std::vector<lsp::Diagnostic> FormatService::Diagnose(std::string_view filePath,
//                                                         std::shared_ptr<LuaParser> parser,
//                                                         std::shared_ptr<LuaCodeStyleOptions> options)
//{
//	auto& settings = _owner->GetSettings();
//	std::vector<lsp::Diagnostic> diagnostics;
//
//	LuaFormatter formatter(parser, *options);
//	formatter.BuildFormattedElement();
//	DiagnosisContext ctx(parser, *options);
//	formatter.CalculateDiagnosisInfos(ctx);
//
//	if (settings.lintCodeStyle)
//	{
//		NameStyleChecker styleChecker(ctx);
//		styleChecker.Analysis();
//	}
//
//	if (settings.spellEnable)
//	{
//		_spellChecker->Analysis(ctx, _customDictionary);
//	}
//
//	ctx.DiagnoseLine();
//
//	auto diagnosisInfos = ctx.GetDiagnosisInfos();
//	for (auto diagnosisInfo : diagnosisInfos)
//	{
//		auto& diagnosis = diagnostics.emplace_back();
//		diagnosis.message = diagnosisInfo.Message;
//		diagnosis.range = lsp::Range(
//                lsp::Position(
//				diagnosisInfo.Range.Start.Line,
//				diagnosisInfo.Range.Start.Character
//			),
//                lsp::Position(
//				diagnosisInfo.Range.End.Line,
//				diagnosisInfo.Range.End.Character
//			));
//		diagnosis.severity = lsp::DiagnosticSeverity::Warning;
//		diagnosis.source = "EmmyLua";
//		switch (diagnosisInfo.type)
//		{
//		case DiagnosisType::Indent:
//		case DiagnosisType::Blank:
//		case DiagnosisType::Align:
//			{
//				diagnosis.code = GetService<CodeActionService>()->GetCode(CodeActionService::DiagnosticCode::Reformat);
//				break;
//			}
//		case DiagnosisType::Spell:
//			{
//				diagnosis.severity = lsp::DiagnosticSeverity::Information;
//				diagnosis.code = GetService<CodeActionService>()->GetCode(CodeActionService::DiagnosticCode::Spell);
//				diagnosis.data = diagnosisInfo.Data;
//				break;
//			}
//		default:
//			{
//				break;
//			}
//		}
//	}
//
//	return diagnostics;
//}

std::string FormatService::Format(LuaSyntaxTree &luaSyntaxTree, LuaStyle &luaStyle) {
    FormatBuilder f(luaStyle);
    f.FormatAnalyze(luaSyntaxTree);
    return f.GetFormatResult(luaSyntaxTree);
}

//std::string FormatService::RangeFormat(LuaFormatRange& range,
//                                           std::shared_ptr<LuaParser> parser,
//                                           LuaCodeStyleOptions& options)
//{
//	LuaFormatter formatter(parser, options);
//	formatter.BuildFormattedElement();
//	return formatter.GetRangeFormattedText(range);
//}
//
//void FormatService::LoadDictionary(std::string_view path)
//{
////	if (_owner->GetSettings().spellEnable)
////	{
////		_spellChecker->LoadDictionary(path);
////	}
//}
//
//void FormatService::SetCustomDictionary(std::vector<std::string>& dictionary)
//{
////	_customDictionary.clear();
////
////	for (auto& word : dictionary)
////	{
////		_customDictionary.insert(word);
////	}
//}
//
//std::shared_ptr<CodeSpellChecker> FormatService::GetSpellChecker()
//{
////	return _spellChecker;
//}

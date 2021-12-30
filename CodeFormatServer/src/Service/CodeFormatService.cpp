#include "CodeFormatServer/Service/CodeFormatService.h"

#include "CodeService/LuaFormatter.h"

CodeFormatService::CodeFormatService(std::shared_ptr<LanguageClient> owner)
	: Service(owner)
{
}

std::vector<vscode::Diagnostic> CodeFormatService::Diagnose(std::string_view filePath,
                                                             std::shared_ptr<LuaParser> parser,
                                                             std::shared_ptr<LuaCodeStyleOptions> options)
{
	LuaFormatter formatter(parser, *options);
	formatter.BuildFormattedElement();

	auto diagnosisInfos = formatter.GetDiagnosisInfos();
	std::vector<vscode::Diagnostic> diagnostics;
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
	}

	std::set<vscode::Range> diagnosticRangeSet;

	for (auto& diagnostic : diagnostics)
	{
		diagnosticRangeSet.insert(diagnostic.range);
	}

	_diagnosticCaches[std::string(filePath)] = std::move(diagnosticRangeSet);

	return diagnostics;
}

std::string CodeFormatService::Format(std::shared_ptr<LuaParser> parser, std::shared_ptr<LuaCodeStyleOptions> options)
{
	LuaFormatter formatter(parser, *options);
	formatter.BuildFormattedElement();

	return formatter.GetFormattedText();
}

bool CodeFormatService::IsDiagnosticRange(std::string_view filePath, vscode::Range& range)
{
	auto it = _diagnosticCaches.find(filePath);

	if (it == _diagnosticCaches.end())
	{
		return false;
	}

	auto& diagnosticSet = it->second;
	if(range.start == range.end)
	{
		for(auto& validRange: diagnosticSet)
		{
			if(validRange.start <= range.start && validRange.end >= range.end)
			{
				return true;
			}
		}
		return false;
	}
	else
	{
		return diagnosticSet.count(range) > 0;
	}
}

std::string CodeFormatService::RangeFormat(LuaFormatRange& range,
                                           std::shared_ptr<LuaParser> parser,
                                           std::shared_ptr<LuaCodeStyleOptions> options)
{
	LuaFormatter formatter(parser, *options);
	formatter.BuildFormattedElement();
	return formatter.GetRangeFormattedText(range);
}

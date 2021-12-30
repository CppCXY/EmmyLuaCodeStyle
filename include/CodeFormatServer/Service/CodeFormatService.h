#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <map>
#include "LuaParser/LuaParser.h"
#include "CodeService/LuaCodeStyleOptions.h"
#include "CodeFormatServer/VSCode.h"
#include "Service.h"
#include "CodeService/LuaFormatRange.h"

class CodeFormatService : public Service
{
public:
	LANGUAGE_SERVICE(CodeFormatService);

	CodeFormatService(std::shared_ptr<LanguageClient> owner);

	std::vector<vscode::Diagnostic> Diagnose(std::string_view filePath, std::shared_ptr<LuaParser> parser,
	                                          std::shared_ptr<LuaCodeStyleOptions> options);

	std::string Format(std::shared_ptr<LuaParser> parser, std::shared_ptr<LuaCodeStyleOptions> options);

	std::string RangeFormat(LuaFormatRange& range, std::shared_ptr<LuaParser> parser,
	                        std::shared_ptr<LuaCodeStyleOptions> options);

	bool IsDiagnosticRange(std::string_view filePath ,vscode::Range& range);
private:
	std::map<std::string, std::set<vscode::Range>, std::less<>> _diagnosticCaches;
};

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
#include "CodeService/Spell/CodeSpellChecker.h"

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

	void MakeSpellActions(std::shared_ptr<vscode::CodeActionResult> result, vscode::Diagnostic& diagnostic, std::string_view uri);

	void LoadDictionary(std::string_view path);

	bool IsCodeFormatDiagnostic(vscode::Diagnostic& diagnostic);
	bool IsSpellDiagnostic(vscode::Diagnostic& diagnostic);
private:

	std::shared_ptr<CodeSpellChecker> _spellChecker;
};

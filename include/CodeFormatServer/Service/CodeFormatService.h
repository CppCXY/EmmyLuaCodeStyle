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

	std::string RangeFormat(LuaFormatRange& range, std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options);

	void LoadDictionary(std::string_view path);

	void SetCustomDictionary(std::vector<std::string>& dictionary);

	std::shared_ptr<CodeSpellChecker> GetSpellChecker();
private:

	std::shared_ptr<CodeSpellChecker> _spellChecker;

	CodeSpellChecker::CustomDictionary _customDictionary;
};

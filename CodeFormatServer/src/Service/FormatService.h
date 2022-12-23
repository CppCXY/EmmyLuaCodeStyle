#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <map>
#include "LSP/LSP.h"
#include "Service.h"
#include "CodeService/Config/LuaStyle.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "CodeService/Format/Types.h"
//#include "CodeService/Spell/CodeSpellChecker.h"

class FormatService : public Service {
public:
    LANGUAGE_SERVICE(FormatService);

    FormatService(LanguageServer *owner);

//	std::vector<lsp::Diagnostic> Diagnose(std::string_view filePath, std::shared_ptr<LuaParser> parser,
//                                          std::shared_ptr<LuaCodeStyleOptions> options);

    std::string Format(LuaSyntaxTree &luaSyntaxTree, LuaStyle &luaStyle);

    std::string RangeFormat(LuaSyntaxTree &luaSyntaxTree, LuaStyle &luaStyle, FormatRange &range);

//	void LoadDictionary(std::string_view path);
//
//	void SetCustomDictionary(std::vector<std::string>& dictionary);
//
//	std::shared_ptr<CodeSpellChecker> GetSpellChecker();
private:

//	std::shared_ptr<CodeSpellChecker> _spellChecker;
//
//	CodeSpellChecker::CustomDictionary _customDictionary;
};

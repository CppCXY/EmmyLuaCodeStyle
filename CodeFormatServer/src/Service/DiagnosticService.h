#pragma once

#include "Service.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "CodeService/Config/LuaStyle.h"
#include "CodeService/Config/LuaDiagnosticStyle.h"
#include "LSP/LSP.h"

class DiagnosticService : public Service {
public:
    LANGUAGE_SERVICE(DiagnosticService);

    explicit DiagnosticService(LanguageServer *owner);

    std::vector<lsp::Diagnostic>
    Diagnostic(
            std::size_t fileId,
            const LuaSyntaxTree &luaSyntaxTree, LuaStyle &luaStyle,
            LuaDiagnosticStyle &diagnosticStyle);

private:
//    std::shared_ptr<CodeSpellChecker> _spellChecker;
//
//    CodeSpellChecker::CustomDictionary _customDictionary;
};


#pragma once

#include "Service.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "CodeFormatCore/Config/LuaStyle.h"
#include "CodeFormatCore/Config/LuaDiagnosticStyle.h"
#include "LSP/LSP.h"
#include "CodeFormatCore/Diagnostic/Spell/CodeSpellChecker.h"
#include "CodeFormatCore/Diagnostic/NameStyle/NameStyleChecker.h"

class DiagnosticService : public Service {
public:
    LANGUAGE_SERVICE(DiagnosticService);

    explicit DiagnosticService(LanguageServer *owner);

    std::vector<lsp::Diagnostic>
    Diagnostic(std::size_t fileId,
               const LuaSyntaxTree &luaSyntaxTree, LuaStyle &luaStyle);

    std::shared_ptr<CodeSpellChecker> GetSpellChecker();

private:
    std::shared_ptr<CodeSpellChecker> _spellChecker;
};


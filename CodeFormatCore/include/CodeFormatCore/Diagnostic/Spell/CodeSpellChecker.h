#pragma once

#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "Util/StringUtil.h"
#include "Util/SymSpell/SymSpell.h"
#include "Util.h"
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

class DiagnosticBuilder;

class CodeSpellChecker {
public:
    using CustomDictionary = std::set<std::string, string_util::CaseInsensitiveLess>;

    CodeSpellChecker();

    void LoadDictionary(std::string_view path);

    void LoadDictionaryFromBuffer(std::string_view buffer);

    void SetCustomDictionary(const CustomDictionary &dictionary);

    void Analyze(DiagnosticBuilder &d, const LuaSyntaxTree &t);

    // copy once
    std::vector<SuggestItem> GetSuggests(std::string word);

private:
    void IdentifyAnalyze(DiagnosticBuilder &d, LuaSyntaxNode &token, const LuaSyntaxTree &t);

    void TextAnalyze(DiagnosticBuilder &d, LuaSyntaxNode &token, const LuaSyntaxTree &t);

    std::shared_ptr<SymSpell> _symSpell;
    CustomDictionary _dictionary;
};

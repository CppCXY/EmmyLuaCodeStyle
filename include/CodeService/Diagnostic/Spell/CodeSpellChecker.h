#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <set>
#include "Util/SymSpell/SymSpell.h"
#include "IdentifyParser.h"
#include "Util/StringUtil.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"

class DiagnosticBuilder;

class CodeSpellChecker {
public:
    using CustomDictionary = std::set<std::string, string_util::CaseInsensitiveLess>;

    CodeSpellChecker();

    void LoadDictionary(std::string_view path);

    void LoadDictionaryFromBuffer(std::string_view buffer);

    void SetCustomDictionary(const CustomDictionary& dictionary);

    void Analyze(DiagnosticBuilder &d, const LuaSyntaxTree &t);

    // copy once
    std::vector<SuggestItem> GetSuggests(std::string word);

private:
    void IdentifyAnalyze(DiagnosticBuilder &d, LuaSyntaxNode &token, const LuaSyntaxTree &t);

    void TextAnalyze(DiagnosticBuilder &d, LuaSyntaxNode &token, const LuaSyntaxTree &t);

    std::shared_ptr<SymSpell> _symSpell;
    std::unordered_map<std::string, std::shared_ptr<spell::IdentifyParser>> _caches;
    CustomDictionary _dictionary;
};


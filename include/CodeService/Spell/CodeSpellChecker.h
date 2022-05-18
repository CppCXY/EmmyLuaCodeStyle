#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include "Util/SymSpell/SymSpell.h"
#include "CodeService/FormatElement/DiagnosisContext.h"
#include "IdentifyParser.h"

class CodeSpellChecker
{
public:
	CodeSpellChecker();

	void LoadDictionary(std::string_view path);

	void LoadDictionaryFromBuffer(std::string_view buffer);

	void Analysis(DiagnosisContext& ctx);

	// copy once
	std::vector<SuggestItem> GetSuggests(std::string word);
private:
	void IdentifyAnalysis(DiagnosisContext& ctx, LuaToken& token);

	std::shared_ptr<SymSpell> _symSpell;
	std::unordered_map<std::string, std::shared_ptr<IdentifyParser>> _caches;
};
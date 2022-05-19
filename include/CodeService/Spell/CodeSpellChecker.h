#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include "Util/SymSpell/SymSpell.h"
#include "CodeService/FormatElement/DiagnosisContext.h"
#include "IdentifyParser.h"
#include "Util/StringUtil.h"

class CodeSpellChecker
{
public:
	using CustomDictionary = std::set<std::string, StringUtil::CaseInsensitiveLess>;

	CodeSpellChecker();

	void LoadDictionary(std::string_view path);

	void LoadDictionaryFromBuffer(std::string_view buffer);

	void Analysis(DiagnosisContext& ctx, const CustomDictionary& customDict = CustomDictionary());

	// copy once
	std::vector<SuggestItem> GetSuggests(std::string word);
private:
	void IdentifyAnalysis(DiagnosisContext& ctx, LuaToken& token, const CustomDictionary& customDict);

	std::shared_ptr<SymSpell> _symSpell;
	std::unordered_map<std::string, std::shared_ptr<spell::IdentifyParser>> _caches;
};

#pragma once

#include <memory>
#include <vector>
#include "Util/SymSpell/SymSpell.h"
#include "CodeService/FormatElement/DiagnosisContext.h"

class CodeSpellChecker
{
public:
	CodeSpellChecker();

	void LoadDictionary(std::string_view path);

	void Analysis(DiagnosisContext& ctx);
private:
	void IdentifyAnalysis(DiagnosisContext& ctx, LuaToken& token);

	std::shared_ptr<SymSpell> _symSpell;
	bool _dictionaryLoaded;
	std::map<std::string, std::vector<SuggestItem>> _caches;
};
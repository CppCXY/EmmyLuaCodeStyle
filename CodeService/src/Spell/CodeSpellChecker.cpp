#include "CodeService/Spell/CodeSpellChecker.h"
#include "LuaParser/LuaTokenTypeDetail.h"
#include "Util/format.h"
#include "LuaDict.hpp"

CodeSpellChecker::CodeSpellChecker()
	: _symSpell(std::make_shared<SymSpell>(SymSpell::Strategy::LazyLoaded))
{
	for (auto& word : LuaBuildInWords)
	{
		_symSpell->CreateDictionaryEntry(word, 1);
	}
}

void CodeSpellChecker::LoadDictionary(std::string_view path)
{
	_symSpell->LoadWordDictionary(std::string(path));
}

void CodeSpellChecker::LoadDictionaryFromBuffer(std::string_view buffer)
{
	 _symSpell->LoadWordDictionaryFromBuffer(buffer);
}

void CodeSpellChecker::Analysis(DiagnosisContext& ctx)
{
	auto parser = ctx.GetParser();
	auto tokenParser = parser->GetTokenParser();
	auto& tokens = tokenParser->GetTokens();
	for (auto& token : tokens)
	{
		if (token.TokenType == TK_NAME)
		{
			IdentifyAnalysis(ctx, token);
		}
	}
}

std::vector<SuggestItem> CodeSpellChecker::GetSuggests(const std::string& word)
{
	return _symSpell->LookUp(word);
}

void CodeSpellChecker::IdentifyAnalysis(DiagnosisContext& ctx, LuaToken& token)
{
	std::shared_ptr<IdentifyParser> parser = nullptr;
	std::string text(token.Text);
	auto it = _caches.find(text);
	if (it != _caches.end())
	{
		parser = it->second;
	}
	else
	{
		parser = std::make_shared<IdentifyParser>(token.Text);
		parser->Parse();
		_caches.insert({ text, parser });
	}

	auto& words = parser->GetWords();
	if (words.empty())
	{
		return;
	}

	for (auto& word : words)
	{
		if (!word.Item.empty() && !_symSpell->IsCorrectWord(word.Item))
		{
			auto range = TextRange(token.Range.StartOffset + word.Range.Start,
			                       token.Range.StartOffset + word.Range.Start + word.Range.Count - 1
			);
			std::string originText(token.Text.substr(word.Range.Start, word.Range.Count));
			ctx.PushDiagnosis(Util::format("Typo: in word '{}'", originText), range, DiagnosisType::Spell, originText);
		}
	}
}

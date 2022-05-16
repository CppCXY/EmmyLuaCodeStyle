#include "CodeService/Spell/CodeSpellChecker.h"
#include "LuaParser/LuaTokenTypeDetail.h"
#include "CodeService/Spell/IdentifyParser.h"
#include "Util/format.h"
#include "LuaDict.hpp"

CodeSpellChecker::CodeSpellChecker()
	: _symSpell(std::make_shared<SymSpell>()),
	  _dictionaryLoaded(false)
{
	for(auto& word: LuaBuildInWords)
	{
		_symSpell->CreateDictionaryEntry(word, 1);
	}
}

void CodeSpellChecker::LoadDictionary(std::string_view path)
{
	_dictionaryLoaded = _symSpell->LoadWordDictionary(std::string(path));
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

void CodeSpellChecker::IdentifyAnalysis(DiagnosisContext& ctx, LuaToken& token)
{
	// 代码标识符分词
	// 全大写的标识符不作检查
	// 
	IdentifyParser parser(token.Text);
	parser.Parse();
	auto& words = parser.GetWords();
	if (words.empty())
	{
		return;
	}

	for (auto& word : words)
	{
		if (!_symSpell->IsCorrectWord(word.Item))
		{
			auto range = TextRange(token.Range.StartOffset + word.Range.Start,
			                       token.Range.StartOffset + word.Range.Count - 1
			);
			ctx.PushDiagnosis(format("Typo '{}'", word.Item), range, DiagnosisType::Spell);
		}
	}
}

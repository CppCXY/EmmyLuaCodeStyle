#include "CodeService/Config/LanguageTranslator.h"

LanguageTranslator& LanguageTranslator::GetInstance()
{
	static LanguageTranslator instance;
	return instance;
}

LanguageTranslator::LanguageTranslator()
{
}

std::string LanguageTranslator::Get(const std::string& source)
{
	auto it = _languageDictionary.find(source);
	if(it != _languageDictionary.end())
	{
		return it->second;
	}
	return source;
}

void LanguageTranslator::SetLanguageMap(std::map<std::string, std::string>&& languageDictionary)
{
	_languageDictionary = languageDictionary;
}

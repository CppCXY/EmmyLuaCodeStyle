#pragma once

#include <string>
#include <map>

class LanguageTranslator
{
public:
	static LanguageTranslator& GetInstance();

	LanguageTranslator();

	std::string Get(const std::string& source);

	void SetLanguageMap(std::map<std::string, std::string>&& languageDictionary);

private:
	std::map<std::string, std::string> _languageDictionary;
};

/*
 * 语言本地化翻译实例，保持一种惯用法
 */
#define LText(text) LanguageTranslator::GetInstance().Get(text)


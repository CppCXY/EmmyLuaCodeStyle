#include "LuaCodeFormat.h"

#include "CodeService/Config/LuaEditorConfig.h"
#include "CodeService/LuaFormatter.h"
#include "CodeService/NameStyle/NameStyleChecker.h"
#include "LuaParser/LuaParser.h"
#include "Util/StringUtil.h"

LuaCodeFormat& LuaCodeFormat::GetInstance()
{
	static LuaCodeFormat instance;
	return instance;
}

LuaCodeFormat::LuaCodeFormat()
	: _defaultOptions(std::make_shared<LuaCodeStyleOptions>()),
	  _codeSpellChecker(std::make_shared<CodeSpellChecker>()),
	  _customParser(std::make_shared<LuaCustomParser>())
{
}

void LuaCodeFormat::UpdateCodeStyle(const std::string& workspaceUri, const std::string& configPath)
{
	auto editorconfig = LuaEditorConfig::LoadFromFile(configPath);

	if (editorconfig == nullptr)
	{
		return;
	}

	for (auto& pair : _editorConfigVector)
	{
		if (pair.first == workspaceUri)
		{
			pair.second = editorconfig;
			pair.second->SetWorkspace(workspaceUri);
			return;
		}
	}

	_editorConfigVector.push_back({
		workspaceUri, editorconfig
	});
	_editorConfigVector.back().second->SetWorkspace(workspaceUri);
}

void LuaCodeFormat::RemoveCodeStyle(const std::string& workspaceUri)
{
	for (auto it = _editorConfigVector.begin(); it != _editorConfigVector.end(); ++it)
	{
		if (it->first == workspaceUri)
		{
			_editorConfigVector.erase(it);
			return;
		}
	}
}

void LuaCodeFormat::SetDefaultCodeStyle(ConfigMap& configMap)
{
	if (!configMap.empty())
	{
		LuaEditorConfig::ParseFromSection(_defaultOptions, configMap);
	}
}

void LuaCodeFormat::SetSupportNonStandardSymbol(const std::string& tokenType, const std::vector<std::string>& tokens)
{
	if (tokenType.size() == 1)
	{
		_customParser->SetTokens(tokenType.front(), tokens);
	}
}

void LuaCodeFormat::LoadSpellDictionary(const std::string& path)
{
	_codeSpellChecker->LoadDictionary(path);
}

void LuaCodeFormat::LoadSpellDictionaryFromBuffer(const std::string& buffer)
{
	_codeSpellChecker->LoadDictionaryFromBuffer(buffer);
}

std::string LuaCodeFormat::Reformat(const std::string& uri, std::string&& text, ConfigMap& configMap)
{
	auto parser = LuaParser::LoadFromBuffer(std::move(text));
	if (_customParser->IsSupportCustomTokens())
	{
		parser->GetTokenParser()->SetCustomParser(_customParser);
	}
	parser->BuildAstWithComment();

	if (parser->HasError())
	{
		return "";
	}
	auto options = GetOptions(uri);
	auto tempOptions = CalculateOptions(uri, configMap);
	LuaFormatter formatter(parser, tempOptions);
	formatter.BuildFormattedElement();
	return formatter.GetFormattedText();
}

std::string LuaCodeFormat::RangeFormat(const std::string& uri, LuaFormatRange& range, std::string&& text,
                                       ConfigMap& configMap)
{
	auto parser = LuaParser::LoadFromBuffer(std::move(text));
	if (_customParser->IsSupportCustomTokens())
	{
		parser->GetTokenParser()->SetCustomParser(_customParser);
	}
	parser->BuildAstWithComment();

	if (parser->HasError())
	{
		return "";
	}
	auto options = GetOptions(uri);

	auto tempOptions = CalculateOptions(uri, configMap);
	LuaFormatter formatter(parser, tempOptions);
	formatter.BuildFormattedElement();

	return formatter.GetRangeFormattedText(range);
}

LuaTypeFormat LuaCodeFormat::TypeFormat(const std::string& uri, int line, int character, std::string&& text,
                                        ConfigMap& configMap, ConfigMap& stringTypeOptions)
{
	auto parser = LuaParser::LoadFromBuffer(std::move(text));
	if (_customParser->IsSupportCustomTokens())
	{
		parser->GetTokenParser()->SetCustomParser(_customParser);
	}
	parser->BuildAstWithComment();
	auto options = GetOptions(uri);
	auto tempOptions = CalculateOptions(uri, configMap);
	auto typeOptions = LuaTypeFormatOptions::ParseFrom(stringTypeOptions);
	LuaTypeFormat typeFormat(parser, tempOptions, typeOptions);
	typeFormat.Analysis("\n", line, character);

	return typeFormat;
}


std::pair<bool, std::vector<LuaDiagnosisInfo>> LuaCodeFormat::Diagnose(const std::string& uri, std::string&& text)
{
	auto parser = LuaParser::LoadFromBuffer(std::move(text));
	if (_customParser->IsSupportCustomTokens())
	{
		parser->GetTokenParser()->SetCustomParser(_customParser);
	}
	parser->BuildAstWithComment();

	if (!parser->GetErrors().empty())
	{
		return std::make_pair(false, std::vector<LuaDiagnosisInfo>());
	}
	auto options = GetOptions(uri);
	LuaFormatter formatter(parser, *options);
	formatter.BuildFormattedElement();

	DiagnosisContext ctx(parser, *options);
	formatter.CalculateDiagnosisInfos(ctx);

	if (options->enable_check_codestyle)
	{
		NameStyleChecker styleChecker(ctx);
		styleChecker.Analysis();
	}

	ctx.DiagnoseLine();

	return std::make_pair(true, ctx.GetDiagnosisInfos());
}

std::vector<LuaDiagnosisInfo> LuaCodeFormat::SpellCheck(const std::string& uri, std::string&& text,
                                                        const CodeSpellChecker::CustomDictionary& tempDict)
{
	auto parser = LuaParser::LoadFromBuffer(std::move(text));
	if (_customParser->IsSupportCustomTokens())
	{
		parser->GetTokenParser()->SetCustomParser(_customParser);
	}
	parser->GetTokenParser()->Parse();

	auto options = GetOptions(uri);

	DiagnosisContext ctx(parser, *options);

	_codeSpellChecker->Analysis(ctx, tempDict);

	return ctx.GetDiagnosisInfos();
}

std::vector<SuggestItem> LuaCodeFormat::SpellCorrect(const std::string& word)
{
	std::string letterWord = word;
	for (auto& c : letterWord)
	{
		c = std::tolower(c);
	}
	bool upperFirst = false;
	if (std::isupper(word.front()))
	{
		upperFirst = true;
	}

	auto suggests = _codeSpellChecker->GetSuggests(letterWord);

	for (auto& suggest : suggests)
	{
		if (!suggest.Term.empty())
		{
			if (upperFirst)
			{
				suggest.Term[0] = std::toupper(suggest.Term[0]);
			}
		}
	}
	return suggests;
}

std::shared_ptr<LuaCodeStyleOptions> LuaCodeFormat::GetOptions(const std::string& uri)
{
	std::size_t matchLength = 0;
	std::shared_ptr<LuaCodeStyleOptions> options = _defaultOptions;
	for (auto it = _editorConfigVector.begin(); it != _editorConfigVector.end(); it++)
	{
		if (StringUtil::StartWith(uri, it->first) && it->first.size() > matchLength)
		{
			matchLength = it->first.size();
			options = it->second->Generate(uri);
		}
	}

	return options;
}

LuaCodeStyleOptions LuaCodeFormat::CalculateOptions(const std::string& uri, ConfigMap& configMap)
{
	auto options = GetOptions(uri);

	if (configMap.empty())
	{
		return *options;
	}
	else
	{
		LuaCodeStyleOptions tempOptions = *options;
		if (configMap.count("insertSpaces"))
		{
			tempOptions.indent_style = configMap.at("insertSpaces") == "true"
				                           ? IndentStyle::Space
				                           : IndentStyle::Tab;
		}
		if (configMap.count("tabSize"))
		{
			if (tempOptions.indent_style == IndentStyle::Tab)
			{
				tempOptions.tab_width = std::stoi(configMap.at("tabSize"));
			}
			else if (tempOptions.indent_style == IndentStyle::Space)
			{
				tempOptions.indent_size = std::stoi(configMap.at("tabSize"));
			}
		}
		return tempOptions;
	}
}

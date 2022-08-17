#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "CodeService/LuaCodeStyleOptions.h"
#include "CodeService/Diagnosis/LuaDiagnosisInfo.h"
#include "CodeService/LuaEditorConfig.h"
#include "CodeService/LuaFormatRange.h"
#include "CodeService/Spell/CodeSpellChecker.h"
#include "CodeService/TypeFormat/LuaTypeFormat.h"

class LuaCodeFormat
{
public:
	using ConfigMap = std::map<std::string, std::string, std::less<>>;
	static LuaCodeFormat& GetInstance();

	LuaCodeFormat();

	void UpdateCodeStyle(const std::string& workspaceUri, const std::string& configPath);
	void RemoveCodeStyle(const std::string& workspaceUri);
	void SetDefaultCodeStyle(ConfigMap& configMap);
	void SetSupportNonStandardSymbol(const std::string& tokenType, const std::vector<std::string>& tokens);

	void LoadSpellDictionary(const std::string& path);

	void LoadSpellDictionaryFromBuffer(const std::string& buffer);

	std::string Reformat(const std::string& uri, std::string&& text, ConfigMap& configMap);

	std::string RangeFormat(const std::string& uri, LuaFormatRange& range, std::string&& text, ConfigMap& configMap);

	LuaTypeFormat TypeFormat(const std::string& uri, int line, int character, std::string&& text,
	                         ConfigMap& configMap, ConfigMap& stringTypeOptions);

	std::pair<bool, std::vector<LuaDiagnosisInfo>> Diagnose(const std::string& uri, std::string&& text);

	std::vector<LuaDiagnosisInfo> SpellCheck(const std::string& uri, std::string&& text,
	                                         const CodeSpellChecker::CustomDictionary& tempDict);

	std::vector<SuggestItem> SpellCorrect(const std::string& word);

	std::shared_ptr<LuaCodeStyleOptions> GetOptions(const std::string& uri);

	LuaCodeStyleOptions CalculateOptions(const std::string& uri, ConfigMap& configMap);
private:
	std::vector<std::pair<std::string, std::shared_ptr<LuaEditorConfig>>> _editorConfigVector;

	std::shared_ptr<LuaCodeStyleOptions> _defaultOptions;

	std::shared_ptr<CodeSpellChecker> _codeSpellChecker;

	std::shared_ptr<LuaCustomParser> _customParser;
};

#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "CodeService/Config/LuaStyle.h"
#include "CodeService/Config/LuaEditorConfig.h"
#include "CodeService/Format/FormatBuilder.h"
#include "CodeService/Diagnostic/Spell/CodeSpellChecker.h"
#include "CodeService/TypeFormat/LuaTypeFormat.h"
#include "CodeService/Diagnostic/DiagnosticBuilder.h"

struct LuaConfig {
    explicit LuaConfig(std::string_view workspace)
            : Workspace(workspace), Editorconfig(nullptr) {}

    std::string Workspace;
    std::shared_ptr<LuaEditorConfig> Editorconfig;
};

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

	std::string RangeFormat(const std::string& uri, FormatRange& range, std::string&& text, ConfigMap& configMap);

	LuaTypeFormat TypeFormat(const std::string& uri, int line, int character, std::string&& text,
	                         ConfigMap& configMap, ConfigMap& stringTypeOptions);

    std::vector<LuaDiagnostic> Diagnostic(const std::string& uri, std::string&& text);

    std::vector<LuaDiagnostic> SpellCheck(const std::string& uri, std::string&& text,
	                                         const CodeSpellChecker::CustomDictionary& tempDict);

	std::vector<SuggestItem> SpellCorrect(const std::string& word);

	LuaStyle& GetStyle(const std::string& uri);
private:
	std::vector<LuaConfig> _configs;
	LuaStyle _defaultStyle;
	std::shared_ptr<CodeSpellChecker> _codeSpellChecker;
};

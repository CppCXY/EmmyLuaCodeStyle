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
#include "Types.h"


class LuaCodeFormat {
public:
    using ConfigMap = std::map<std::string, std::string, std::less<>>;

    static LuaCodeFormat &GetInstance();

    LuaCodeFormat();

    void UpdateCodeStyle(const std::string &workspaceUri, const std::string &configPath);

    void RemoveCodeStyle(const std::string &workspaceUri);

    void SetDefaultCodeStyle(ConfigMap &configMap);

    void SetSupportNonStandardSymbol(const std::string &tokenType, const std::vector<std::string> &tokens);

    void LoadSpellDictionary(const std::string &path);

    void LoadSpellDictionaryFromBuffer(const std::string &buffer);

    std::string Reformat(const std::string &uri, std::string &&text, ConfigMap &configMap);

    std::string RangeFormat(const std::string &uri, FormatRange &range, std::string &&text, ConfigMap &configMap);

    std::vector<LuaTypeFormat::Result>
    TypeFormat(const std::string &uri, std::size_t line, std::size_t character, std::string &&text,
               ConfigMap &configMap, ConfigMap &stringTypeOptions);

    std::vector<LuaDiagnosticInfo> Diagnostic(const std::string &uri, std::string &&text);

    std::vector<LuaDiagnosticInfo> SpellCheck(const std::string &uri, std::string &&text,
                                          const CodeSpellChecker::CustomDictionary &tempDict);

    std::vector<LuaDiagnosticInfo> NameStyleCheck(const std::string &uri, std::string &&text);

    std::vector<SuggestItem> SpellCorrect(const std::string &word);

    LuaStyle &GetStyle(const std::string &uri);
private:
    std::vector<LuaDiagnosticInfo> MakeDiagnosticInfo(const std::vector<LuaDiagnostic>& diagnostics,
                                                      std::shared_ptr<LuaFile> file);

    std::vector<LuaConfig> _configs;
    LuaStyle _defaultStyle;
    std::shared_ptr<CodeSpellChecker> _spellChecker;
};

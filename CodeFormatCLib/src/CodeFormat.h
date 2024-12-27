#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "CodeFormatCore/Config/LuaEditorConfig.h"
#include "CodeFormatCore/Config/LuaStyle.h"
#include "CodeFormatCore/Diagnostic/DiagnosticBuilder.h"
#include "CodeFormatCore/Diagnostic/Spell/CodeSpellChecker.h"
#include "CodeFormatCore/Format/FormatBuilder.h"
#include "CodeFormatCore/TypeFormat/LuaTypeFormat.h"
#include "UtilTypes.h"
#include "CodeFormatCLib.h"

class CodeFormat {
public:
    using ConfigMap = std::map<std::string, std::string, std::less<>>;

    static CodeFormat &GetInstance();

    CodeFormat();

    void UpdateCodeStyle(const std::string &workspaceUri, const std::string &configPath);

    void UpdateDiagnosticStyle(InfoTree &tree);

    void RemoveCodeStyle(const std::string &workspaceUri);

    void SetDefaultCodeStyle(ConfigMap &configMap);

    void SupportNonStandardSymbol();

    char *Reformat(const std::string &uri, std::string &&text);

    Result<RangeFormatResult> RangeFormat(const std::string &uri, FormatRange &range, std::string &&text);

    void SupportCLikeComments();// 添加支持 C 语言注释的函数

    Result<std::vector<LuaTypeFormat::Result>>
    TypeFormat(const std::string &uri, std::size_t line, std::size_t character, std::string &&text);

    Result<std::vector<LuaDiagnosticInfo>> Diagnostic(const std::string &uri, std::string &&text);

    LuaStyle &GetStyle(const std::string &uri);

private:
    std::vector<LuaDiagnosticInfo> MakeDiagnosticInfo(const std::vector<LuaDiagnostic> &diagnostics,
                                                      std::shared_ptr<LuaSource> file);

    std::vector<LuaConfig> _configs;
    LuaStyle _defaultStyle;
    CodeSpellChecker _spellChecker;
    LuaDiagnosticStyle _diagnosticStyle;
    bool _supportNonStandardSymbol;
    bool _supportCLikeComments;
};

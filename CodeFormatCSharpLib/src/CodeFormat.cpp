#include "CodeFormat.h"

#include "CodeFormatCore/Config/LuaEditorConfig.h"
#include "CodeFormatCore/RangeFormat/RangeFormatBuilder.h"
#include "LuaParser/Parse/LuaParser.h"
#include "Util/StringUtil.h"

CodeFormat &CodeFormat::GetInstance() {
    static CodeFormat instance;
    return instance;
}

CodeFormat::CodeFormat()
    : _supportNonStandardSymbol(false),
      _supportCLikeComments(false) {
}

void CodeFormat::UpdateCodeStyle(const std::string &workspaceUri, const std::string &configPath) {
    for (auto &config: _configs) {
        if (config.Workspace == workspaceUri) {
            config.Editorconfig = LuaEditorConfig::OpenFile(configPath);
            return;
        }
    }

    auto &config = _configs.emplace_back(
            workspaceUri);
    config.Editorconfig = LuaEditorConfig::OpenFile(configPath);
}

void CodeFormat::UpdateDiagnosticStyle(InfoTree &tree) {
    _diagnosticStyle = LuaDiagnosticStyle();
    _diagnosticStyle.ParseTree(tree);
    _diagnosticStyle.code_style_check = true;
    _diagnosticStyle.name_style_check = false;
    _diagnosticStyle.spell_check = false;
}

void CodeFormat::RemoveCodeStyle(const std::string &workspaceUri) {
    for (auto it = _configs.begin(); it != _configs.end(); it++) {
        if (it->Workspace == workspaceUri) {
            _configs.erase(it);
            break;
        }
    }
}

void CodeFormat::SetDefaultCodeStyle(ConfigMap &configMap) {
    if (!configMap.empty()) {
        LuaStyle style;
        style.Parse(configMap);
        _defaultStyle = style;
    }
}

void CodeFormat::SupportNonStandardSymbol() {
    _supportNonStandardSymbol = true;
}

Result<char*> CodeFormat::Reformat(const std::string &uri, std::string &&text) {
    auto file = std::make_shared<LuaSource>(std::move(text));
    LuaLexer luaLexer(file);
    if (_supportNonStandardSymbol) {
        luaLexer.SupportNonStandardSymbol();
    }
    if (_supportCLikeComments) {
        luaLexer.SupportCLikeComments();
    }

    luaLexer.Parse();

    LuaParser p(file, std::move(luaLexer.GetTokens()));
    p.Parse();

    if (p.HasError()) {
        return ResultType::Err;
    }

    LuaSyntaxTree t;
    t.BuildTree(p);

    LuaStyle style = GetStyle(uri);

    FormatBuilder f(style);

    auto result = f.GetFormatResult(t);
    // 由于可能存在sso string，所以需要拷贝一份
    char* ptr = new char[result.size() + 1];
    std::copy(result.begin(), result.end(), ptr);
    ptr[result.size()] = '\0'; // [result.size()] = '\0'
    return ptr;
}

Result<RangeFormatResult> CodeFormat::RangeFormat(const std::string &uri, FormatRange &range,
                                            std::string &&text) {
    auto file = std::make_shared<LuaSource>(std::move(text));
    LuaLexer luaLexer(file);
    if (_supportNonStandardSymbol) {
        luaLexer.SupportNonStandardSymbol();
    }
    if (_supportCLikeComments) {
        luaLexer.SupportCLikeComments();
    }

    luaLexer.Parse();

    LuaParser p(file, std::move(luaLexer.GetTokens()));
    p.Parse();

    if (p.HasError()) {
        return ResultType::Err;
    }

    LuaSyntaxTree t;
    t.BuildTree(p);

    LuaStyle style = GetStyle(uri);

    RangeFormatBuilder f(style, range);

    auto formattedText = f.GetFormatResult(t);
    range = f.GetReplaceRange();

    char* ptr = new char[formattedText.size() + 1];
    std::copy(formattedText.begin(), formattedText.end(), ptr);
    ptr[formattedText.size()] = '\0'; // [formattedText.size()] = '\0'
    return RangeFormatResult{
        static_cast<int>(range.StartLine),
        static_cast<int>(range.StartCol),
        static_cast<int>(range.EndLine),
        static_cast<int>(range.EndCol),
        ptr
    };
}

Result<std::vector<LuaTypeFormat::Result>>
CodeFormat::TypeFormat(const std::string &uri, std::size_t line, std::size_t character, std::string &&text) {
    auto file = std::make_shared<LuaSource>(std::move(text));
    LuaLexer luaLexer(file);
    if (_supportNonStandardSymbol) {
        luaLexer.SupportNonStandardSymbol();
    }
    if (_supportCLikeComments) {
        luaLexer.SupportCLikeComments();
    }

    luaLexer.Parse();

    LuaParser p(file, std::move(luaLexer.GetTokens()));
    p.Parse();

    if (p.HasError()) {
        return ResultType::Err;
    }

    LuaSyntaxTree t;
    t.BuildTree(p);

    LuaStyle style = GetStyle(uri);


    LuaTypeFormatFeatures typeFormatOptions;
    typeFormatOptions.auto_complete_end = false;
    LuaTypeFormat tf(typeFormatOptions);
    tf.Analyze("\n", line, character, t, style);
    return tf.GetResult();
}

Result<std::vector<LuaDiagnosticInfo>> CodeFormat::Diagnostic(const std::string &uri, std::string &&text) {
    auto file = std::make_shared<LuaSource>(std::move(text));
    LuaLexer luaLexer(file);
    if (_supportNonStandardSymbol) {
        luaLexer.SupportNonStandardSymbol();
    }
    if (_supportCLikeComments) {
        luaLexer.SupportCLikeComments();
    }

    luaLexer.Parse();

    LuaParser p(file, std::move(luaLexer.GetTokens()));
    p.Parse();

    if (p.HasError()) {
        return ResultType::Err;
    }

    LuaSyntaxTree t;
    t.BuildTree(p);

    LuaStyle style = GetStyle(uri);

    DiagnosticBuilder diagnosticBuilder(style, _diagnosticStyle);
    diagnosticBuilder.CodeStyleCheck(t);
    return MakeDiagnosticInfo(diagnosticBuilder.GetDiagnosticResults(t), file);
}

LuaStyle &CodeFormat::GetStyle(const std::string &uri) {
    std::shared_ptr<LuaEditorConfig> editorConfig = nullptr;
    std::size_t matchProcess = 0;
    for (auto &config: _configs) {
        if (string_util::StartWith(uri, config.Workspace)) {
            if (config.Workspace.size() > matchProcess) {
                matchProcess = config.Workspace.size();
                editorConfig = config.Editorconfig;
            }
        }
    }

    if (editorConfig) {
        return editorConfig->Generate(uri);
    }
    return _defaultStyle;
}

std::vector<LuaDiagnosticInfo> CodeFormat::MakeDiagnosticInfo(const std::vector<LuaDiagnostic> &diagnostics,
                                                              std::shared_ptr<LuaSource> file) {
    std::vector<LuaDiagnosticInfo> results;
    for (auto &diagnostic: diagnostics) {
        auto &result = results.emplace_back();
        result.Type = diagnostic.Type;
        result.Message = diagnostic.Message;
        result.Data = diagnostic.Data;
        result.Start.Line = file->GetLine(diagnostic.Range.StartOffset);
        result.Start.Col = file->GetColumn(diagnostic.Range.StartOffset);
        result.End.Line = file->GetLine(diagnostic.Range.GetEndOffset());
        result.End.Col = file->GetColumn(diagnostic.Range.GetEndOffset()) + 1;
    }

    return results;
}

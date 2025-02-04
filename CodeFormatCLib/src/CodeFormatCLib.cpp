#include "CodeFormatCLib.h"
#include "CodeFormat.h"
#include "UtilTypes.h"

extern "C" {

EMMY_API char *ReformatLuaCode(const char *code, const char *uri) {
    CodeFormat &codeFormat = CodeFormat::GetInstance();
    auto result = codeFormat.Reformat(uri, code);
    return result;
}

EMMY_API RangeFormatResult RangeFormatLuaCode(const char *code, const char *uri, int startLine, int startCol, int endLine, int endCol) {
    CodeFormat &codeFormat = CodeFormat::GetInstance();
    FormatRange range;
    range.StartLine = startLine;
    range.StartCol = startCol;
    range.EndLine = endLine;
    range.EndCol = endCol;
    auto result = codeFormat.RangeFormat(uri, range, code);
    if (result.Type == ResultType::Ok) {
        return result.Data;
    } else {
        return RangeFormatResult{};
    }
}

EMMY_API void FreeReformatResult(char *ptr) {
    delete[] ptr;
}

EMMY_API void UpdateCodeStyle(const char *workspaceUri, const char *configPath) {
    CodeFormat &codeFormat = CodeFormat::GetInstance();
    codeFormat.UpdateCodeStyle(workspaceUri, configPath);
}

EMMY_API void RemoveCodeStyle(const char *workspaceUri) {
    CodeFormat &codeFormat = CodeFormat::GetInstance();
    codeFormat.RemoveCodeStyle(workspaceUri);
}

EMMY_API char *CheckCodeStyle(const char *code, const char *uri) {
    CodeFormat &codeFormat = CodeFormat::GetInstance();
    auto result = codeFormat.Diagnostic(uri, code);
    if (result.Type != ResultType::Ok) {
        return nullptr;
    }
    auto diagnostic_results = std::move(result.Data);
    std::string result_str;
    auto total_size = 0;
    for (auto &diagnostic: diagnostic_results) {
        total_size += diagnostic.Message.size();
    }
    result_str.reserve(total_size * 2);

    for (auto &diagnostic: diagnostic_results) {
        result_str.append(std::to_string(diagnostic.Start.Line)).append("|");
        result_str.append(std::to_string(diagnostic.Start.Col)).append("|");
        result_str.append(std::to_string(diagnostic.End.Line)).append("|");
        result_str.append(std::to_string(diagnostic.End.Col)).append("|");
        result_str.append(diagnostic.Message);
        result_str.append("\n");
    }
    auto ptr = new char[result_str.size() + 1];
    std::copy(result_str.begin(), result_str.end(), ptr);
    ptr[result_str.size()] = '\0';
    return ptr;
}
}
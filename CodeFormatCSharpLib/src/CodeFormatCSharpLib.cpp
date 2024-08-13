#include "CodeFormat.h"

#if defined(_WIN32)
#define EMMY_API __declspec(dllexport)
#elif defined(__GNUC__)
#define EMMY_API __attribute__((visibility("default")))
#else
#define EMMY_API
#endif


extern "C" {

EMMY_API char *ReformatLuaCode(const char *code, const char *uri) {
    CodeFormat &codeFormat = CodeFormat::GetInstance();
    auto result = codeFormat.Reformat(uri, code);
    if (result.Type == ResultType::Ok) {
        return result.Data;
    } else {
        return nullptr;
    }
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
}
#include "CodeFormat.h"
#include "CodeFormatCLib.h"
#include "Types.h"

#if defined(_WIN32)
#define EMMY_API __declspec(dllexport)
#elif defined(__GNUC__)
#define EMMY_API __attribute__((visibility("default")))
#else
#define EMMY_API
#endif


extern "C" {

// 定义不透明指针结构
struct CodeFormatHandle {
    CodeFormat* instance;
};

// 创建 CodeFormat 实例
EMMY_API CodeFormatHandle* CreateCodeFormat() {
    CodeFormatHandle* handle = new CodeFormatHandle();
    handle->instance = &CodeFormat::GetInstance();
    return handle;
}

// 销毁 CodeFormat 实例
EMMY_API void DestroyCodeFormat(CodeFormatHandle* handle) {
    delete handle;
}

EMMY_API char *ReformatLuaCode(CodeFormatHandle* handle, const char *code, const char *uri) {
    CodeFormat &codeFormat = *handle->instance;
    auto result = codeFormat.Reformat(uri, code);
    if (result.Type == ResultType::Ok) {
        return result.Data;
    } else {
        return nullptr;
    }
}

EMMY_API RangeFormatResult RangeFormatLuaCode(CodeFormatHandle* handle, const char *code, const char *uri, int startLine, int startCol, int endLine, int endCol) {
    CodeFormat &codeFormat = *handle->instance;
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

EMMY_API void UpdateCodeStyle(CodeFormatHandle* handle, const char *workspaceUri, const char *configPath) {
    CodeFormat &codeFormat = *handle->instance;
    codeFormat.UpdateCodeStyle(workspaceUri, configPath);
}

EMMY_API void RemoveCodeStyle(CodeFormatHandle* handle, const char *workspaceUri) {
    CodeFormat &codeFormat = *handle->instance;
    codeFormat.RemoveCodeStyle(workspaceUri);
}
}
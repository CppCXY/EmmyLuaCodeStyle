#ifndef CODE_FORMAT_C_LIB_H
#define CODE_FORMAT_C_LIB_H

#include <stdint.h> // 使用固定大小的整数类型

#if defined(_WIN32)
#define EMMY_API __declspec(dllexport)
#elif defined(__GNUC__)
#define EMMY_API __attribute__((visibility("default")))
#else
#define EMMY_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

// 使用固定大小的类型，并确保结构体按字节对齐
typedef struct RangeFormatResult {
    int32_t StartLine;
    int32_t StartCharacter;
    int32_t EndLine;
    int32_t EndCharacter;
    char *Text;
} RangeFormatResult;

// 定义不透明指针类型
typedef struct CodeFormatHandle CodeFormatHandle;

// 创建和销毁 CodeFormat 实例
EMMY_API CodeFormatHandle* CreateCodeFormat();
EMMY_API void DestroyCodeFormat(CodeFormatHandle* handle);

// 修改函数使用不透明指针
EMMY_API char* ReformatLuaCode(CodeFormatHandle* handle, const char *code, const char *uri);
EMMY_API RangeFormatResult RangeFormatLuaCode(CodeFormatHandle* handle, const char *code, const char *uri, int32_t startLine, int32_t startCol, int32_t endLine, int32_t endCol);
EMMY_API void FreeReformatResult(char *ptr);
EMMY_API void UpdateCodeStyle(CodeFormatHandle* handle, const char *workspaceUri, const char *configPath);
EMMY_API void RemoveCodeStyle(CodeFormatHandle* handle, const char *workspaceUri);

#ifdef __cplusplus
}
#endif

#endif// CODE_FORMAT_C_LIB_H
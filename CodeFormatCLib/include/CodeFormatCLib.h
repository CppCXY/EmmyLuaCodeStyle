#ifndef CODE_FORMAT_C_LIB_H
#define CODE_FORMAT_C_LIB_H

#include <cstdint>// 使用固定大小的整数类型

#if defined(_WIN32)
#define EMMY_API __declspec(dllexport)
#elif defined(__GNUC__)
#define EMMY_API __attribute__((visibility("default")))
#else
#define EMMY_API
#endif

extern "C" {

struct RangeFormatResult {
    int32_t StartLine;
    int32_t StartCharacter;
    int32_t EndLine;
    int32_t EndCharacter;
    char *Text;
};

EMMY_API char *ReformatLuaCode(const char *code, const char *uri);

EMMY_API RangeFormatResult RangeFormatLuaCode(const char *code, const char *uri, int startLine, int startCol, int endLine, int endCol);

EMMY_API void FreeReformatResult(char *ptr);

EMMY_API void UpdateCodeStyle(const char *workspaceUri, const char *configPath);

EMMY_API void RemoveCodeStyle(const char *workspaceUri);
}


#endif// CODE_FORMAT_C_LIB_H
#pragma once

#include <cstdint> // 使用固定大小的整数类型
#include <cstddef> // 对于指针大小
#include <string_view>

#include "CodeFormatCore/Config/LuaEditorConfig.h"
#include "CodeFormatCore/Config/LuaStyle.h"
#include "CodeFormatCore/Diagnostic/DiagnosticBuilder.h"
#include "CodeFormatCore/Diagnostic/Spell/CodeSpellChecker.h"
#include "CodeFormatCore/Format/FormatBuilder.h"
#include "CodeFormatCore/TypeFormat/LuaTypeFormat.h"

struct LuaConfig {
    explicit LuaConfig(std::string_view workspace)
        : Workspace(workspace), Editorconfig(nullptr) {}

    std::string Workspace;
    std::shared_ptr<LuaEditorConfig> Editorconfig;
};

struct Position {
    uint32_t Line;
    uint32_t Col;
};

struct LuaDiagnosticInfo {
    DiagnosticType Type;
    Position Start;
    Position End;
    std::string Message;
    std::string Data;
};

enum class ResultType {
    Ok,
    Err
};

template<class T>
class Result {
public:
    Result(T &&d)
        : Type(ResultType::Ok), Data(d) {}

    Result(ResultType type)
        : Type(type) {}

    ResultType Type;
    T Data;
};

struct RangeFormatResult {
    int32_t StartLine;
    int32_t StartCharacter;
    int32_t EndLine;
    int32_t EndCharacter;
    char *Text;
};
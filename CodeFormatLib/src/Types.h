#pragma once

struct LuaConfig {
    explicit LuaConfig(std::string_view workspace)
            : Workspace(workspace), Editorconfig(nullptr) {}

    std::string Workspace;
    std::shared_ptr<LuaEditorConfig> Editorconfig;
};

struct Position {
    std::size_t Line;
    std::size_t Col;
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
            : Data(d) {}

    Result(ResultType type)
            : Type(type) {}

    ResultType Type;
    T Data;
};

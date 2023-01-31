#pragma once

#include <string_view>
#include <memory>
#include "CodeService/Config/LuaEditorConfig.h"


struct LuaConfig {
    explicit LuaConfig(std::string_view workspace)
            : Workspace(workspace), Editorconfig(nullptr) {}

    std::string Workspace;
    std::shared_ptr<LuaEditorConfig> Editorconfig;
};

enum class WorkMode {
    File,
    Workspace
};

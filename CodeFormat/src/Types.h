#pragma once

#include "CodeFormatCore/Config/LuaEditorConfig.h"
#include <memory>
#include <string_view>


struct LuaConfig {
    explicit LuaConfig(std::string_view workspace)
        : Workspace(workspace), Editorconfig(nullptr) {}

    std::string Workspace;
    std::shared_ptr<LuaEditorConfig> Editorconfig;
};

enum class WorkMode {
    File,
    Stdin,
    Workspace
};

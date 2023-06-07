#pragma once

#include <string_view>
#include <memory>
#include "CodeFormatCore/Config/LuaEditorConfig.h"


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

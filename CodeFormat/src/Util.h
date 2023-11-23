#pragma once

#include "CodeFormatCore/Config/LuaEditorConfig.h"
#include <memory>
#include <string_view>
#include <optional>
#include <string>


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

std::optional<std::string> ReadFile(std::string_view path);
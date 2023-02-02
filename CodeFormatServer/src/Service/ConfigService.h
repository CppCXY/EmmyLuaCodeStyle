#pragma once

#include <vector>
#include "LSP/LSP.h"
#include "Service.h"
#include "CodeService/Config/LuaEditorConfig.h"
#include "Config/ClientConfig.h"
#include "CodeService/Config/LuaDiagnosticStyle.h"

struct LuaConfig {
    explicit LuaConfig(std::string_view workspace)
            : Workspace(workspace), Editorconfig(nullptr) {}

    std::string Workspace;
    std::shared_ptr<LuaEditorConfig> Editorconfig;
};

class ConfigService : public Service {
public:
    LANGUAGE_SERVICE(ConfigService);

    explicit ConfigService(LanguageServer *owner);

    LuaStyle &GetLuaStyle(std::string_view fileUri);

    void LoadEditorconfig(std::string_view workspace, std::string_view filePath);

    void RemoveEditorconfig(std::string_view workspace);

    void LoadLanguageTranslator(std::string_view filePath);

    void UpdateClientConfig(ClientConfig clientConfig);

    LuaDiagnosticStyle &GetDiagnosticStyle();

private:
    std::vector<LuaConfig> _styleConfigs;
    LuaStyle _defaultStyle;
    LuaDiagnosticStyle _diagnosticStyle;
};
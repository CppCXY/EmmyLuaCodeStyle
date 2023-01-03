#include "ConfigService.h"
#include <fstream>
#include "CodeService/Config/LanguageTranslator.h"

ConfigService::ConfigService(LanguageServer *owner)
        : Service(owner) {

}

LuaStyle &ConfigService::GetLuaStyle(std::string_view fileUri) {
    std::shared_ptr<LuaEditorConfig> editorConfig = nullptr;
    std::size_t matchProcess = 0;
    for (auto &config: _styleConfigs) {
        if (fileUri.starts_with(config.Workspace)) {
            if (config.Workspace.size() > matchProcess) {
                matchProcess = config.Workspace.size();
                editorConfig = config.Editorconfig;
            }
        }
    }

    if (editorConfig) {
        return editorConfig->Generate(fileUri);
    }
    return _defaultStyle;
}

void ConfigService::LoadEditorconfig(std::string_view workspace, std::string_view filePath) {
    std::string path(filePath);
    for (auto &config: _styleConfigs) {
        if (config.Workspace == workspace) {
            config.Editorconfig = LuaEditorConfig::LoadFromFile(path);
            config.Editorconfig->Parse();
            break;
        }
    }

    auto &config = _styleConfigs.emplace_back(
            std::string(workspace)
    );
    config.Editorconfig = LuaEditorConfig::LoadFromFile(path);
    config.Editorconfig->Parse();
}

void ConfigService::LoadLanguageTranslator(std::string_view filePath) {
    std::string path(filePath);
    std::fstream fin(path, std::ios::in);

    if (fin.is_open()) {
        std::stringstream s;
        s << fin.rdbuf();
        std::string jsonText = s.str();
        auto json = nlohmann::json::parse(jsonText);

        if (json.is_object()) {
            std::map<std::string, std::string> languageMap;
            for (auto [key, value]: json.items()) {
                languageMap.insert({key, value});
            }

            LanguageTranslator::GetInstance().SetLanguageMap(std::move(languageMap));
        }
    }
}

void ConfigService::RemoveEditorconfig(std::string_view workspace) {
    for (auto it = _styleConfigs.begin(); it != _styleConfigs.end(); it ++) {
        if (it->Workspace == workspace) {
            _styleConfigs.erase(it);
            break;
        }
    }
}



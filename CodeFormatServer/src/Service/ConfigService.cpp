#include "ConfigService.h"
#include "CodeFormatCore/Config/LanguageTranslator.h"
#include "DiagnosticService.h"
#include "LanguageServer.h"
#include "Util/Url.h"
#include <fstream>
#include <sstream>

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
        auto filePath = url::UrlToFilePath(fileUri);
        return editorConfig->Generate(filePath);
    }
    return _defaultStyle;
}

void ConfigService::LoadEditorconfig(std::string_view workspace, std::string_view filePath) {
    std::string path(filePath);
    for (auto &config: _styleConfigs) {
        if (config.Workspace == workspace) {
            config.Editorconfig = LuaEditorConfig::LoadFromFile(path);
            config.Editorconfig->Parse();
            return;
        }
    }

    auto &config = _styleConfigs.emplace_back(
            std::string(workspace));
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
    for (auto it = _styleConfigs.begin(); it != _styleConfigs.end(); it++) {
        if (it->Workspace == workspace) {
            _styleConfigs.erase(it);
            break;
        }
    }
}

void ConfigService::UpdateClientConfig(ClientConfig clientConfig) {

    LuaDiagnosticStyle diagnosticStyle;
    diagnosticStyle.ParseTree(clientConfig.configTree);
    _diagnosticStyle = diagnosticStyle;

    CodeSpellChecker::CustomDictionary dictionary(clientConfig.emmylua_spell_dict.begin(),
                                                  clientConfig.emmylua_spell_dict.end());
    auto diagnosticService = _owner->GetService<DiagnosticService>();
    diagnosticService->GetSpellChecker()->SetCustomDictionary(dictionary);
}

LuaDiagnosticStyle &ConfigService::GetDiagnosticStyle() {
    return _diagnosticStyle;
}

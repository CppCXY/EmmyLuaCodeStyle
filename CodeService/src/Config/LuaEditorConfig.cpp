
#include "CodeService/Config/LuaEditorConfig.h"

#include <sstream>
#include <fstream>
#include <regex>

#include "Util/StringUtil.h"

std::shared_ptr<LuaEditorConfig> LuaEditorConfig::LoadFromFile(const std::string &path) {
    std::fstream fin(path, std::ios::in);
    if (fin.is_open()) {
        std::stringstream s;
        s << fin.rdbuf();
        auto editorConfig = std::make_shared<LuaEditorConfig>(s.str());
        return editorConfig;
    }

    return nullptr;
}

LuaEditorConfig::LuaEditorConfig(std::string &&source)
        : _source(source) {
}

void LuaEditorConfig::Parse() {
    auto lines = string_util::Split(_source, "\n");

    std::regex comment = std::regex(R"(^\s*(;|#))");
    std::regex luaSection = std::regex(R"(^\s*\[\s*([^\]]+)\s*\]\s*$)");
    std::regex valueRegex = std::regex(R"(^\s*([\w\d_]+)\s*=\s*(.+)$)");
    bool sectionFounded = false;

    for (auto &lineView: lines) {
        std::string line(lineView);
        if (std::regex_search(line, comment)) {
            continue;
        }

        std::smatch m;

        if (std::regex_search(line, m, luaSection)) {
            auto pattern = m.str(1);
            sectionFounded = (pattern.find("lua") != std::string::npos) || pattern == "*";
            _sections.emplace_back(pattern);
            continue;
        }

        if (sectionFounded) {
            if (std::regex_search(line, m, valueRegex)) {
                _sections.back().ConfigMap.insert({m.str(1), std::string(string_util::TrimSpace(m.str(2)))});
            }
        }
    }
}

LuaStyle &LuaEditorConfig::Generate(std::string_view filePath) {

    std::vector<std::size_t> patternSection;
    for (std::size_t i = 0; i != _sections.size(); i++) {
        auto &pattern = _sections[i].Pattern;

        // [*] [*.lua]
        if (pattern.GetPattern() == "*" || pattern.GetPattern() == "*.lua") {
            patternSection.push_back(i);
        }
            // [{test.lua,lib.lua}]
        else if (pattern.Match(filePath)) {
            patternSection.push_back(i);
        }
    }

    std::string patternKey;
    patternKey.reserve(64);
    for (auto i: patternSection) {
        patternKey.append(std::to_string(i)).push_back('#');
    }

    auto it = _styleMap.find(patternKey);
    if (it != _styleMap.end()) {
        return it->second;
    }

    _styleMap.insert({patternKey, LuaStyle()});
    auto &luaStyle = _styleMap.at(patternKey);
    for (auto i: patternSection) {
        auto &configMap = _sections[i].ConfigMap;
        luaStyle.ParseFromMap(configMap);
    }

    return luaStyle;
}

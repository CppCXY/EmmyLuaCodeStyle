
#include "CodeFormatCore/Config/LuaEditorConfig.h"

#include <fstream>
#include <regex>
#include <sstream>

#include "Util/StringUtil.h"

#ifdef WIN32
#include <Windows.h>
std::wstring utf8ToWideChar(const std::string &utf8Str) {
    int wideCharSize = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, NULL, 0);
    std::wstring wideStr(wideCharSize, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wideStr[0], wideCharSize);
    return wideStr;
}
#endif

std::shared_ptr<LuaEditorConfig> LuaEditorConfig::OpenFile(const std::string &path) {
    //FIX windows下读取中文路径
#ifdef WIN32
    std::fstream fin(utf8ToWideChar(path), std::ios::in | std::ios::binary);
#else
    std::fstream fin(path, std::ios::in | std::ios::binary);
#endif
    if (fin.is_open()) {
        fin.seekg(0, std::ios::end);
        auto size = fin.tellg();
        std::string s(size, ' ');
        fin.seekg(0);
        fin.read(s.data(), size);
        auto config = std::make_shared<LuaEditorConfig>();
        config->Parse(s);
        return config;
    }

    return nullptr;
}

static bool IsWhiteSpaces(int c) {
    return c > 0 && std::isspace(c);
}

static bool IsEndOfLine(int c){
    return c == '\r' || c == '\n';
}

void LuaEditorConfig::Parse(std::string_view text) {
    auto reader = TextReader(text);
    _sections.emplace_back("");
    while (!reader.IsEof()) {
        reader.ResetBuffer();
        switch (reader.GetCurrentChar()) {
                // comment
            case ';':
            case '#':
                //skip empty line
            case '\r':
            case '\n': {
                reader.NextLine();
                break;
            }
            case '\t':
            case ' ': {
                reader.EatWhile(IsWhiteSpaces);
                break;
            }
            case '[': {
                reader.NextChar();
                reader.EatWhile([](char ch) { return ch != ']' && !IsEndOfLine(ch); });
                auto pattern = string_util::TrimSpace(reader.GetSaveText());
                if (pattern.empty() || reader.GetCurrentChar() != ']') {
                    reader.NextLine();
                    break;
                }
                reader.NextLine();
                _sections.emplace_back(pattern);
                break;
            }
            default: {
                reader.EatWhile([](char ch) { return ch != '=' && !IsEndOfLine(ch); });
                auto key = string_util::TrimSpace(reader.GetSaveText());
                if (key.empty() || reader.GetCurrentChar() != '=') {
                    reader.NextLine();
                    break;
                }

                reader.NextChar();
                reader.ResetBuffer();
                reader.EatWhile([](char ch) { return !IsEndOfLine(ch); });
                auto value = string_util::TrimSpace(reader.GetSaveText());
                if (value.empty()) {
                    reader.NextLine();
                    break;
                }
                _sections.back().ConfigMap.insert({std::string(key), std::string(value)});
                break;
            }
        }
    }
}

LuaStyle &LuaEditorConfig::Generate(std::string_view filePath) {

    std::vector<std::size_t> patternSection;
    for (std::size_t i = 0; i != _sections.size(); i++) {
        auto &pattern = _sections[i].Pattern;

        // "" [*] [*.lua]
        if (pattern.GetPattern().empty() || pattern.GetPattern() == "*" || pattern.GetPattern() == "*.lua") {
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
        if (!configMap.empty()) {
            luaStyle.Parse(configMap);
        }
    }

    return luaStyle;
}

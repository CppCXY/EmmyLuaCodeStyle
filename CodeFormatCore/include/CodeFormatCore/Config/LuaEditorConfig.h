#pragma once

#include <map>
#include <string>
#include <memory>
#include "LuaStyle.h"
#include "EditorconfigPattern.h"

class LuaEditorConfig {
public:
    class Section {
    public:
        explicit Section(std::string_view pattern) {
            Pattern.Compile(pattern);
        }

        EditorconfigPattern Pattern;
        std::map<std::string, std::string, std::less<>> ConfigMap;
    };

    static std::shared_ptr<LuaEditorConfig> LoadFromFile(const std::string &path);

    LuaEditorConfig(std::string &&source);

    void Parse();

    LuaStyle &Generate(std::string_view fileUri);

private:

    std::string _source;
    std::vector<Section> _sections;
    std::map<std::string, LuaStyle, std::less<>> _styleMap;
};

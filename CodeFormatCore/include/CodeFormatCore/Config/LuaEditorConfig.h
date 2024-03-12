#pragma once

#include "EditorconfigPattern.h"
#include "LuaStyle.h"
#include <map>
#include <memory>
#include <string>

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

    static std::shared_ptr<LuaEditorConfig> OpenFile(const std::string &path);

    LuaStyle &Generate(std::string_view fileUri);

private:
    void Parse(std::string_view text);

    std::vector<Section> _sections;
    std::map<std::string, LuaStyle, std::less<>> _styleMap;
};

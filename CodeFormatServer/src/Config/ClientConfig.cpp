#include "ClientConfig.h"

InfoNode CreateFromJson(InfoTree &t, nlohmann::json json) {
    if (json.is_object()) {
        auto object = t.CreateObject();
        for (auto &[k, v]: json.items()) {
            object.AddChild(k, CreateFromJson(t, v));
        }
        return object;
    } else if (json.is_array()) {
        auto arr = t.CreateArray();
        for (auto v: json) {
            arr.AddChild(CreateFromJson(t, v));
        }
        return arr;
    } else if (json.is_number()) {
        return t.CreateNumber(json);
    } else if (json.is_string()) {
        return t.CreateString(json);
    } else if (json.is_boolean()) {
        return t.CreateBool(json);
    } else {
        return t.CreateNone();
    }
}

void ClientConfig::Deserialize(nlohmann::json json) {
    auto root = configTree.GetRoot();

    if (json["emmylua"].is_object()) {
        auto emmylua = json["emmylua"];
        if (emmylua["lint"].is_object()) {
            auto lint = emmylua["lint"];
            if (lint["codeStyle"].is_boolean()) {
                root.AddChild("code_style_check", bool(lint["codeStyle"]));
            }
            if (lint["nameStyle"].is_boolean()) {
                root.AddChild("name_style_check", bool(lint["nameStyle"]));
            }
            if (lint["spellCheck"].is_boolean()) {
                root.AddChild("spell_check", bool(lint["spellCheck"]));
            }
        }

        if (emmylua["spell"].is_object()) {
            auto spell = emmylua["spell"];
            if (spell["dict"].is_array()) {
                emmylua_spell_dict.clear();
                for (auto j: spell["dict"]) {
                    if (j.is_string()) {
                        emmylua_spell_dict.push_back(j);
                    }
                }
            }
        }

        if (emmylua["name"].is_object()) {
            auto name = emmylua["name"];
            if (name["config"].is_object()) {
                for (auto &[key, valueJson]: name["config"].items()) {
                    root.AddChild(key, CreateFromJson(configTree, valueJson));
                }
            }
        }
    }
}

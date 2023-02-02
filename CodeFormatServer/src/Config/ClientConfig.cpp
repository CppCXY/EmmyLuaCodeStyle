#include "ClientConfig.h"

void ClientConfig::Deserialize(nlohmann::json json) {
    if (json["emmylua"].is_object()) {
        auto emmylua = json["emmylua"];
        if (emmylua["lint"].is_object()) {
            auto lint = emmylua["lint"];
            if (lint["codeStyle"].is_boolean()) {
                emmylua_lint_codeStyle = lint["codeStyle"];
            }
            if (lint["nameStyle"].is_boolean()) {
                emmylua_lint_nameStyle = lint["nameStyle"];
            }
        }

        if (emmylua["spell"].is_object()) {
            auto spell = emmylua["spell"];
            if (spell["enable"].is_boolean()) {
                emmylua_spell_enable = spell["enable"];
            }

            if (spell["dict"].is_array()) {
                emmylua_spell_dict.clear();
                for (auto j: spell["dict"]) {
                    if (j.is_string()) {
                        emmylua_spell_dict.push_back(j);
                    }
                }
            }
        }
    }
}

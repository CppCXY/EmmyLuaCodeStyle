#include "CodeFormatCore/Config/LuaDiagnosticStyle.h"
#include "Util/StringUtil.h"

#define BOOL_OPTION(op)                            \
    if (auto n = root.GetValue(#op); n.IsBool()) { \
        op = n.AsBool();                           \
    }

NameStyleRule MakeNameStyle(InfoNode n) {
    if (n.IsString()) {
        auto type = n.AsString();
        if (type == "snake_case") {
            return NameStyleRule(NameStyleType::SnakeCase);
        } else if (type == "upper_snake_case") {
            return NameStyleRule(NameStyleType::UpperSnakeCase);
        } else if (type == "pascal_case") {
            return NameStyleRule(NameStyleType::PascalCase);
        } else if (type == "camel_case") {
            return NameStyleRule(NameStyleType::CamelCase);
        }

    } else if (n.IsObject()) {
        auto typeNode = n.GetValue("type");
        if (typeNode.IsString()) {
            auto type = typeNode.AsString();
            if (type == "snake_case") {
                return NameStyleRule(NameStyleType::SnakeCase);
            } else if (type == "upper_snake_case") {
                return NameStyleRule(NameStyleType::UpperSnakeCase);
            } else if (type == "pascal_case") {
                return NameStyleRule(NameStyleType::PascalCase);
            } else if (type == "camel_case") {
                return NameStyleRule(NameStyleType::CamelCase);
            } else if (type == "ignore") {
                auto paramNode = n.GetValue("param");
                std::set<std::string> ignores;
                if (paramNode.IsString()) {
                    ignores.insert(paramNode.AsString());
                } else if (paramNode.IsArray()) {
                    auto arr = paramNode.AsArray();
                    for(auto item : arr){
                        if(item.IsString()) {
                            ignores.insert(item.AsString());
                        }
                    }
                }

                return NameStyleRule(NameStyleType::Ignore, std::make_shared<IgnoreNameStyleData>(ignores));
            } else if (type == "pattern") {
                auto paramNode = n.GetValue("param");
                if (paramNode.IsString()) {

                    auto patternData = std::make_shared<PatternNameStyleData>();
                    auto patternString = paramNode.AsString();
                    try {
                        patternData->Re = std::regex(patternString, std::regex_constants::ECMAScript);
                        patternData->PatternString = patternString;
                        for (auto pair: n.AsMap()) {
                            if (string_util::StartWith(pair.first, "$")) {
                                auto istr = pair.first.substr(1);
                                std::size_t groupId = std::stoull(istr);
                                auto rule = pair.second.AsString();

                                if (rule == "snake_case") {
                                    patternData->GroupRules.emplace_back(groupId, NameStyleType::SnakeCase);
                                } else if (rule == "upper_snake_case") {
                                    patternData->GroupRules.emplace_back(groupId, NameStyleType::UpperSnakeCase);
                                } else if (rule == "pascal_case") {
                                    patternData->GroupRules.emplace_back(groupId, NameStyleType::PascalCase);
                                } else if (rule == "camel_case") {
                                    patternData->GroupRules.emplace_back(groupId, NameStyleType::CamelCase);
                                }
                            }
                        }

                        return NameStyleRule(NameStyleType::Pattern, patternData);
                    } catch (std::exception &) {
                        return NameStyleRule(NameStyleType::Off);
                    }
                }
            } else {
                return NameStyleRule(NameStyleType::Off);
            }
        }
    }
    return NameStyleRule(NameStyleType::Off);
}

void LuaDiagnosticStyle::ParseTree(InfoTree &tree) {
    auto root = tree.GetRoot();

    BOOL_OPTION(code_style_check);

    BOOL_OPTION(name_style_check);

    BOOL_OPTION(spell_check);

    std::vector<std::pair<std::vector<NameStyleRule> &, std::string>> name_styles = {
            {local_name_style,           "local_name_style"          },
            {function_param_name_style,  "function_param_name_style" },
            {function_name_style,        "function_name_style"       },
            {local_function_name_style,  "local_function_name_style" },
            {table_field_name_style,     "table_field_name_style"    },
            {global_variable_name_style, "global_variable_name_style"},
            {module_name_style,          "module_name_style"         },
            {require_module_name_style,  "require_module_name_style" },
            {class_name_style,           "class_name_style"          }
    };
    for (auto &pair: name_styles) {
        if (auto n = root.GetValue(pair.second); !n.IsNull()) {
            pair.first.clear();
            if (n.IsArray()) {
                for (auto c: n.AsArray()) {
                    pair.first.push_back(MakeNameStyle(c));
                }
            } else {
                pair.first.push_back(MakeNameStyle(n));
            }
        }
    }
}

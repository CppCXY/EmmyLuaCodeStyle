#pragma once

#include <regex>
#include <string>
#include <set>

enum class NameStyleType {
    Off,
    CamelCase,
    PascalCase,
    SnakeCase,
    UpperSnakeCase,
    Ignore,
    Pattern,
};

struct NameStyleData {
    virtual ~NameStyleData(){};
};

struct NameStyleRule {
    explicit NameStyleRule(NameStyleType type)
        : Type(type) {}

    NameStyleRule(NameStyleType type, std::shared_ptr<NameStyleData> data)
        : Type(type), Data(data) {}


    NameStyleType Type;
    std::shared_ptr<NameStyleData> Data;
};

struct IgnoreNameStyleData : public NameStyleData {
    explicit IgnoreNameStyleData(const std::set<std::string>& param)
        : NameStyleData(), Param(param.begin(), param.end()) {}
    std::set<std::string, std::less<>> Param;
};

struct PatternNameStyleData : public NameStyleData {
    struct Group {
        Group()
            : GroupId(0), Rule(NameStyleType::Off) {}

        Group(std::size_t groupId, NameStyleType rule)
            : GroupId(groupId), Rule(rule) {}

        std::size_t GroupId;
        NameStyleType Rule;
    };

    std::regex Re;
    std::string PatternString;
    std::vector<Group> GroupRules;
};
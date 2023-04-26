#pragma once

#include <regex>
#include <string>

enum class NameStyleType {
    Off,
    CamelCase,
    PascalCase,
    SnakeCase,
    UpperSnakeCase,
    Same,
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

struct SameNameStyleData : public NameStyleData {
    explicit SameNameStyleData(std::string_view param)
        : NameStyleData(), Param(param) {}
    std::string Param;
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
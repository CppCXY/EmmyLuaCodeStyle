#pragma once

#include "InfoKind.h"
#include "InfoNode.h"
#include <string>
#include <vector>
#include <unordered_map>

class InfoTree {
public:
    struct InfoOrNode {
        InfoKind Kind;
        union {
            std::size_t ChildIndex;
            std::size_t StringIndex;
            double NumberValue;
            int IntValue;
            bool BoolValue;
        } Data;
    };

    InfoTree();

    InfoNode GetRoot();

    InfoNode CreateObject();

    InfoNode CreateArray();

    InfoNode CreateString(std::string_view s);

    friend InfoNode;
private:
    std::vector<InfoOrNode> _nodeOrInfos;
    std::vector<std::unordered_map<std::string, std::size_t>> _mapChildren;
    std::vector<std::vector<std::size_t>> _arrayChildren;
    std::vector<std::string> _stringValues;
};

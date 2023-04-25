#pragma once

#include "InfoKind.h"
#include <string>
#include <unordered_map>

class InfoTree;

class InfoNode {
public:
    InfoNode(std::size_t index, InfoTree *tree);

    InfoKind GetKind() const;

    bool IsNull() const;

    bool IsObject() const;

    bool IsArray() const;

    bool IsNumber() const;

    bool IsString() const;

    bool IsBool() const;

    void AddChild(InfoNode n);

    void AddChild(std::string_view key, InfoNode n);

    std::string AsString() const;

    double AsDouble() const;

    int AsInt() const;

    bool AsBool() const;

    std::vector<InfoNode> GetChildren() const;

    InfoNode GetValue(std::string_view key) const;
private:
    std::size_t _index;
    InfoTree *_tree;
};

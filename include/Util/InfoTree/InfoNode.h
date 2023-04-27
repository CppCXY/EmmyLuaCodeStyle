#pragma once

#include "InfoKind.h"
#include <string>
#include <vector>
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

    void AddChild(std::string s);

    void AddChild(bool b);

    void AddChild(double d);

    void AddChild(std::string_view key, InfoNode n);

    void AddChild(std::string_view key, std::string s);

    void AddChild(std::string_view key, bool b);

    void AddChild(std::string_view key, double d);

    std::string AsString() const;

    double AsDouble() const;

    int AsInt() const;

    bool AsBool() const;

    std::vector<InfoNode> AsArray() const;

    std::unordered_map<std::string, InfoNode> AsMap() const;

    InfoNode GetValue(std::string_view key) const;
private:
    std::size_t _index;
    InfoTree *_tree;
};

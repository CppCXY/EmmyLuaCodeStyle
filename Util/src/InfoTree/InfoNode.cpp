#include "Util/InfoTree/InfoNode.h"
#include "Util/InfoTree/InfoTree.h"

InfoNode::InfoNode(std::size_t index, InfoTree *tree)
    : _index(index), _tree(tree) {
}

InfoKind InfoNode::GetKind() const {
    if (_index < _tree->_nodeOrInfos.size()) {
        return _tree->_nodeOrInfos[_index].Kind;
    }
    return InfoKind::None;
}

bool InfoNode::IsNull() const {
    return _index == 0;
}

bool InfoNode::IsObject() const {
    return GetKind() == InfoKind::Object;
}

bool InfoNode::IsArray() const {
    return GetKind() == InfoKind::Array;
}

bool InfoNode::IsNumber() const {
    return GetKind() == InfoKind::Number;
}

bool InfoNode::IsString() const {
    return GetKind() == InfoKind::String;
}

bool InfoNode::IsBool() const {
    return GetKind() == InfoKind::Bool;
}

void InfoNode::AddChild(InfoNode n) {
    if (_index < _tree->_nodeOrInfos.size()) {
        auto node = _tree->_nodeOrInfos[_index];
        if (node.Kind == InfoKind::Array) {
            _tree->_arrayChildren[node.Data.ChildIndex].push_back(n._index);
        }
    }
}

void InfoNode::AddChild(std::string s) {
    auto c = _tree->CreateString(s);
    AddChild(c);
}

void InfoNode::AddChild(bool b) {
    auto c = _tree->CreateBool(b);
    AddChild(c);
}

void InfoNode::AddChild(double d) {
    auto c = _tree->CreateNumber(d);
    AddChild(c);
}

void InfoNode::AddChild(std::string_view key, InfoNode n) {
    if (_index < _tree->_nodeOrInfos.size()) {
        auto node = _tree->_nodeOrInfos[_index];
        if (node.Kind == InfoKind::Object) {
            _tree->_mapChildren[node.Data.ChildIndex].insert({std::string(key), n._index});
        }
    }
}

void InfoNode::AddChild(std::string_view key, std::string s) {
    auto c = _tree->CreateString(s);
    AddChild(key, c);
}

void InfoNode::AddChild(std::string_view key, bool b) {
    auto c = _tree->CreateBool(b);
    AddChild(key, c);
}

void InfoNode::AddChild(std::string_view key, double d) {
    auto c = _tree->CreateNumber(d);
    AddChild(key, c);
}

std::string InfoNode::AsString() const {
    if (_index < _tree->_nodeOrInfos.size()) {
        if (_tree->_nodeOrInfos[_index].Kind == InfoKind::String) {
            return _tree->_stringValues[_tree->_nodeOrInfos[_index].Data.StringIndex];
        }
    }
    return "";
}

double InfoNode::AsDouble() const {
    if (_index < _tree->_nodeOrInfos.size()) {
        auto &node = _tree->_nodeOrInfos[_index];
        if (node.Kind == InfoKind::Number) {
            return node.Data.NumberValue;
        }
    }
    return 0.0;
}

int InfoNode::AsInt() const {
    if (_index < _tree->_nodeOrInfos.size()) {
        auto &node = _tree->_nodeOrInfos[_index];
        if (node.Kind == InfoKind::Number) {
            return node.Data.IntValue;
        }
    }
    return 0;
}

bool InfoNode::AsBool() const {
    if (_index < _tree->_nodeOrInfos.size()) {
        auto &node = _tree->_nodeOrInfos[_index];
        if (node.Kind == InfoKind::Bool) {
            return node.Data.BoolValue;
        }
    }
    return false;
}

std::vector<InfoNode> InfoNode::AsArray() const {
    std::vector<InfoNode> result;
    if (_index < _tree->_nodeOrInfos.size()) {
        auto &node = _tree->_nodeOrInfos[_index];
        if (node.Kind == InfoKind::Array) {
            for (auto i: _tree->_arrayChildren[node.Data.ChildIndex]) {
                result.emplace_back(i, _tree);
            }
        }
    }
    return result;
}

std::unordered_map<std::string, InfoNode> InfoNode::AsMap() const {
    std::unordered_map<std::string, InfoNode> umap;
    if (_index < _tree->_nodeOrInfos.size()) {
        auto &node = _tree->_nodeOrInfos[_index];
        if (node.Kind == InfoKind::Object) {
            auto &innerUmap = _tree->_mapChildren[node.Data.ChildIndex];
            for (auto &p: innerUmap) {
                umap.insert({p.first, InfoNode(p.second, _tree)});
            }
        }
    }
    return umap;
}

InfoNode InfoNode::GetValue(std::string_view key) const {
    if (_index < _tree->_nodeOrInfos.size()) {
        auto &node = _tree->_nodeOrInfos[_index];
        if (node.Kind == InfoKind::Object) {
            auto &umap = _tree->_mapChildren[node.Data.ChildIndex];
            std::string keyString(key);
            auto it = umap.find(keyString);
            if (it != umap.end()) {
                return InfoNode(it->second, _tree);
            }
        }
    }
    return InfoNode(0, _tree);
}
#include "Util/InfoTree/InfoTree.h"

InfoTree::InfoTree() {
    InfoOrNode n;
    n.Kind = InfoKind::None;
    n.Data.ChildIndex = 0;
    _nodeOrInfos.push_back(n);
}

InfoNode InfoTree::CreateNone() {
    auto nodeIndex = _nodeOrInfos.size();

    InfoOrNode n;
    n.Kind = InfoKind::None;
    n.Data.ChildIndex = 0;
    _nodeOrInfos.push_back(n);

    return InfoNode(nodeIndex, this);
}

InfoNode InfoTree::CreateObject() {
    auto nodeIndex = _nodeOrInfos.size();
    auto childIndex = _mapChildren.size();
    _mapChildren.emplace_back();

    InfoOrNode n;
    n.Kind = InfoKind::Object;
    n.Data.ChildIndex = childIndex;
    _nodeOrInfos.push_back(n);

    return InfoNode(nodeIndex, this);
}

InfoNode InfoTree::CreateArray() {
    auto nodeIndex = _nodeOrInfos.size();
    auto childIndex = _arrayChildren.size();
    _arrayChildren.emplace_back();

    InfoOrNode n;
    n.Kind = InfoKind::Array;
    n.Data.ChildIndex = childIndex;
    _nodeOrInfos.push_back(n);

    return InfoNode(nodeIndex, this);
}

InfoNode InfoTree::CreateString(std::string s) {
    auto nodeIndex = _nodeOrInfos.size();
    auto valueIndex = _stringValues.size();
    _stringValues.emplace_back(s);

    InfoOrNode n;
    n.Kind = InfoKind::String;
    n.Data.StringIndex = valueIndex;
    _nodeOrInfos.push_back(n);

    return InfoNode(nodeIndex, this);
}

InfoNode InfoTree::CreateBool(bool b) {
    auto nodeIndex = _nodeOrInfos.size();
    InfoOrNode n;
    n.Kind = InfoKind::Bool;
    n.Data.BoolValue = b;
    _nodeOrInfos.push_back(n);

    return InfoNode(nodeIndex, this);
}

InfoNode InfoTree::CreateNumber(double d) {
    auto nodeIndex = _nodeOrInfos.size();
    InfoOrNode n;
    n.Kind = InfoKind::Bool;
    n.Data.NumberValue = d;
    _nodeOrInfos.push_back(n);

    return InfoNode(nodeIndex, this);
}

InfoNode InfoTree::GetRoot() {
    if (_nodeOrInfos.size() <= 1) {
        CreateObject();
    }
    return InfoNode(1, this);
}

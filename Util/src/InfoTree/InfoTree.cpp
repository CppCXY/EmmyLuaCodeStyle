#include "Util/InfoTree/InfoTree.h"

InfoTree::InfoTree() {
    InfoOrNode n;
    n.Kind = InfoKind::None;
    n.Data.ChildIndex = 0;
    _nodeOrInfos.push_back(n);
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

InfoNode InfoTree::CreateString(std::string_view s) {
    auto nodeIndex = _nodeOrInfos.size();
    auto valueIndex = _stringValues.size();
    _arrayChildren.emplace_back();

    InfoOrNode n;
    n.Kind = InfoKind::String;
    n.Data.StringIndex = valueIndex;
    _nodeOrInfos.push_back(n);

    return InfoNode(nodeIndex, this);
}

InfoNode InfoTree::GetRoot() {
    if (_nodeOrInfos.size() <= 1) {
        CreateObject();
    }
    return InfoNode(1, this);
}

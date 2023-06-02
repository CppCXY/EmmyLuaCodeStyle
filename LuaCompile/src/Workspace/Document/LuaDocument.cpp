#include "LuaDocument.h"

LuaDocument::LuaDocument(LuaDocument::DocumentId id)
    : _id(id) {
}

void LuaDocument::Initialize(std::string &&text) {
    _tree = LuaSyntaxTree::ParseText(std::move(text));
}

LuaSyntaxTree *LuaDocument::GetSyntaxTree() {
    if (!_tree) {
        return nullptr;
    }

    return _tree.get();
}

#pragma once

#include "Lua/SyntaxTree/LuaSyntaxTree.h"
#include <memory>

class LuaDocument {
public:
    using DocumentId = std::size_t;

    explicit LuaDocument(DocumentId id = 0);

    void Initialize(std::string&& text);

    LuaSyntaxTree* GetSyntaxTree();

private:
    DocumentId _id;
    std::unique_ptr<LuaSyntaxTree> _tree;
};

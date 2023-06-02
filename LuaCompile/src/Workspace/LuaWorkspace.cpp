#include "LuaWorkspace.h"
#include "Lua/Source/LuaSource.h"
#include "Lua/Source/SourceUpdateEvent.h"
#include "Lua/SyntaxTree/TreeUpdateEvent.h"

LuaWorkspace::LuaWorkspace()
    : _idCounter(0) {
}

void LuaWorkspace::AddDocument(LuaDocument::DocumentId id, std::string &&text) {
    auto ptr = std::make_unique<LuaDocument>(id);
    ptr->Initialize(std::move(text));
    _documents[id] = std::move(ptr);
}

void LuaWorkspace::RemoveDocument(LuaDocument::DocumentId id) {
    auto it = _documents.find(id);
    if (it != _documents.end()) {
        _documents.erase(it);
    }
}

LuaDocument *LuaWorkspace::GetDocument(LuaDocument::DocumentId id) {
    auto it = _documents.find(id);
    if (it != _documents.end()) {
        return it->second.get();
    }
    return nullptr;
}

LuaDocument::DocumentId LuaWorkspace::AllocDocumentId() {
    return _idCounter++;
}

void LuaWorkspace::UpdateDocument(LuaDocument::DocumentId id, const lsp::Range &range, std::string &&text) {
    auto doc = GetDocument(id);
    if (!doc) {
        return;
    }

    auto tree = doc->GetSyntaxTree();
    auto& source = tree->GetSource();
    auto fileUpdateEvent = SourceUpdateEvent::From(source.GetLineIndex(), range, std::move(text));
    auto treeUpdateEvent = TreeUpdateEvent::From(fileUpdateEvent, *tree);

    tree->ApplyUpdate(treeUpdateEvent);
}

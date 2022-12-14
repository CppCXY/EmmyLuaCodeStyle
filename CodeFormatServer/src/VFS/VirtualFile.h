#pragma once

#include "LSP/LSP.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "DB/FileDB.h"
#include "Lib/LineIndex/LineIndex.h"

class VirtualFileSystem;

class VirtualFile
{
public:
	explicit VirtualFile(std::size_t fieldId);

    bool IsNull() const;

    std::optional<LuaSyntaxTree> GetSyntaxTree(VirtualFileSystem& vfs) const;

    std::shared_ptr<LineIndex> GetLineIndex(VirtualFileSystem& vfs) const;
private:
	std::size_t _fileId;
};
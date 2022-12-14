#pragma once

#include "DBBase.h"
#include <string>
#include <vector>
#include "LSP/LSP.h"

class FileDB : public SharedDBBase<std::size_t, std::string> {
public:
    FileDB();

    std::size_t AllocFileId();

    void ApplyFileUpdate(std::size_t fileId, std::string &&text);

    void ApplyFileUpdate(std::vector<lsp::TextDocumentContentChangeEvent>& changeEvent);

private:
    std::size_t _fileIdCounter;
};

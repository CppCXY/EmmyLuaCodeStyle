#include "FileDB.h"

FileDB::FileDB()
        : SharedDBBase<std::size_t, std::string>(), _fileIdCounter(1) {

}

std::size_t FileDB::AllocFileId() {
    return _fileIdCounter++;
}

void FileDB::ApplyFileUpdate(std::size_t fileId, std::string &&text) {
    auto ptr = std::make_shared<std::string>(std::move(text));
    Input(fileId, std::move(ptr));
}

void FileDB::ApplyFileUpdate(std::vector<lsp::TextDocumentContentChangeEvent> &changeEvent) {

}

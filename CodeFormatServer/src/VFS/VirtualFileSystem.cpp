#include "VirtualFileSystem.h"

VirtualFileSystem::VirtualFileSystem() {

}

void VirtualFileSystem::UpdateFile(std::string_view uri, const lsp::Range &range, std::string &&text) {
    std::string uriString(uri);
    auto opFileId = _uriDB.Query(uriString);
    if (!opFileId.has_value()) {
        auto fileId = _fileDB.AllocFileId();
        opFileId = fileId;
        _uriDB.Input(uriString, std::move(fileId));
    }

    UpdateFile(opFileId.value(), range, std::move(text));
}

void VirtualFileSystem::UpdateFile(std::string_view uri, std::string &&text) {
    std::string uriString(uri);
    auto opFileId = _uriDB.Query(uriString);
    if (!opFileId.has_value()) {
        auto fileId = _fileDB.AllocFileId();
        opFileId = fileId;
        _uriDB.Input(uriString, std::move(fileId));
    }

    _fileDB.ApplyFileUpdate(opFileId.value(), std::move(text));
}

void VirtualFileSystem::UpdateFile(std::size_t fileId, const lsp::Range &range, std::string &&text) {
    auto opSourceText = _fileDB.Query(fileId);
    if (!opSourceText.has_value()) {
        _fileDB.ApplyFileUpdate(fileId, std::move(text));
        return;
    }

    auto vFile = VirtualFile(fileId);
    auto lineIndex = vFile.GetLineIndex(*this);
    if (!lineIndex) {
        _fileDB.ApplyFileUpdate(fileId, std::move(text));
        return;
    }

    auto &sourceText = *opSourceText.value();
    auto startOffset = lineIndex->GetOffset(LineCol(range.start.line, range.start.character));
    auto endOffset = lineIndex->GetOffset(LineCol(range.end.line, range.end.character));

    auto oldSize = sourceText.size();
    auto newSize = oldSize + (text.size() - (endOffset - startOffset));
    if (newSize > sourceText.capacity()) {
        auto suitableCapacity = newSize + 4096;
        sourceText.reserve(suitableCapacity);
    }

    // for insert
    if (startOffset == endOffset) {
        sourceText.insert(startOffset, text);
    }
        // for replace
    else {
        sourceText.replace(startOffset, endOffset - startOffset, text);
    }

    lineIndex->Parse(sourceText);
    _fileDB.ApplyFileUpdate(fileId, std::move(sourceText));
}

void
VirtualFileSystem::UpdateFile(std::string_view uri, std::vector<lsp::TextDocumentContentChangeEvent> &changeEvent) {
    std::string stringUri(uri);
    auto opFileId = _uriDB.Query(stringUri);
    if (!opFileId.has_value()) {
        return;
    }
    auto fileId = opFileId.value();
    auto opSourceText = _fileDB.Query(fileId);
    if (!opSourceText.has_value()) {
        return;
    }
    auto &sourceText = *opSourceText.value();

    auto vFile = VirtualFile(fileId);
    auto lineIndex = vFile.GetLineIndex(*this);
    if (!lineIndex) {
        return;
    }

    std::string text;
    auto totalSize = sourceText.size();
    std::vector<std::pair<TextRange, std::string>> textChanges;
    for (auto &change: changeEvent) {
        if (!change.range.has_value()) {
            return;
        }
        auto range = change.range.value();
        auto &content = change.text;
        auto startOffset = lineIndex->GetOffset(LineCol(range.start.line, range.start.character));
        auto endOffset = lineIndex->GetOffset(LineCol(range.end.line, range.end.character));
        textChanges.emplace_back(TextRange(startOffset, endOffset), std::move(content));
        totalSize += content.size() - (endOffset - startOffset);
    }

    std::stable_sort(textChanges.begin(), textChanges.end(), [](auto &x, auto &y) -> bool {
        return x.first.StartOffset < y.first.StartOffset;
    });

    if (totalSize > 0) {
        text.reserve(totalSize);
        std::size_t start = 0;
        for (std::size_t index = 0; index != textChanges.size(); index++) {
            auto textRange = textChanges[index].first;
            if (start < textRange.StartOffset) {
                text.append(sourceText.begin() + start, sourceText.begin() + textRange.StartOffset);
            }

            auto &content = textChanges[index].second;

            text.append(content);

            start = textChanges[index].first.EndOffset;
        }

        if (start < sourceText.size()) {
            text.append(sourceText.begin() + start, sourceText.end());
        }
    }

    lineIndex->Parse(text);
    _fileDB.ApplyFileUpdate(fileId, std::move(text));
}

void VirtualFileSystem::ClearFile(std::string_view uri) {
    std::string stringUri(uri);
    auto opFileId = _uriDB.Query(stringUri);
    if (opFileId.has_value()) {
        auto fieldId = opFileId.value();
        _uriDB.Delete(stringUri);
        _fileDB.Delete(fieldId);
        _lineIndexDB.Delete(fieldId);
    }
}
//
//void LanguageServer::DiagnosticFile(std::string_view uri, std::string_view prevId,
//                                    std::shared_ptr<lsp::DocumentDiagnosticReport> report) {
//    report->kind = lsp::DocumentDiagnosticReportKind::Unchanged;
//    auto filename = url::UrlToFilePath(uri);
//    auto it = _fileMap.find(filename);
//    if (it == _fileMap.end()) {
//        return;
//    }
//    auto &virtualFile = it->second;
//
//    auto resultId = std::to_string(_configVersion) + "|" + std::to_string(virtualFile->GetVersion());
//    report->resultId = resultId;
//    if (resultId == prevId) {
//        return;
//    }
//
//    auto options = GetOptions(uri);
//    if (!options->enable_check_codestyle) {
//        report->kind = lsp::DocumentDiagnosticReportKind::Full;
//        return;
//    }
//
//    std::shared_ptr<LuaParser> parser = it->second->GetLuaParser();
//
//    if (parser->HasError()) {
//        report->kind = lsp::DocumentDiagnosticReportKind::Unchanged;
//        return;
//    }
//
//    report->kind = lsp::DocumentDiagnosticReportKind::Full;
//
//    // TODO move spell check to SpellService
//    auto formatDiagnostic = GetService<FormatService>()->Diagnose(filename, parser, options);
//    std::copy(formatDiagnostic.begin(), formatDiagnostic.end(), std::back_inserter(report->items));
//
//    if (_vscodeSettings.lintModule) {
//        auto moduleDiagnosis = GetService<ModuleService>()->Diagnose(filename, parser);
//        std::copy(moduleDiagnosis.begin(), moduleDiagnosis.end(), std::back_inserter(report->items));
//    }
//}

VirtualFile VirtualFileSystem::GetVirtualFile(std::string_view uri) {
    std::string stringUri(uri);
    auto opFileId = _uriDB.Query(stringUri);
    if (opFileId.has_value()) {
        return VirtualFile(opFileId.value());
    }
    return VirtualFile(0);
}

UriDB &VirtualFileSystem::GetUriDB() {
    return _uriDB;
}

FileDB &VirtualFileSystem::GetFileDB() {
    return _fileDB;
}

LineIndexDB &VirtualFileSystem::GetLineIndexDB() {
    return _lineIndexDB;
}

VirtualFile VirtualFileSystem::GetVirtualFile(std::size_t fieldId) {
    return VirtualFile(fieldId);
}

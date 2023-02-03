#include "VirtualFile.h"
#include "Util/Url.h"
#include "LuaParser/Lexer/LuaLexer.h"
#include "LuaParser/Parse/LuaParser.h"
#include "Lib/LineIndex/LineIndex.h"
#include "VirtualFileSystem.h"

VirtualFile::VirtualFile(std::size_t fileId)
        : _fileId(fileId) {
}

std::optional<LuaSyntaxTree> VirtualFile::GetSyntaxTree(VirtualFileSystem &vfs) const {
    auto &db = vfs.GetFileDB();
    auto opText = db.Query(_fileId);
    if (opText.has_value()) {
        auto text = *std::move(opText.value());
        auto file = std::make_shared<LuaFile>(std::move(text));
        LuaLexer luaLexer(file);
        luaLexer.Parse();

        LuaParser p(file, std::move(luaLexer.GetTokens()));
        p.Parse();

        LuaSyntaxTree t;
        t.BuildTree(p);
        return t;
    }
    return std::nullopt;
}

std::shared_ptr<LineIndex> VirtualFile::GetLineIndex(VirtualFileSystem &vfs) const {
    auto &lineIndexDB = vfs.GetLineIndexDB();
    auto &fileDB = vfs.GetFileDB();
    auto opLineIndex = lineIndexDB.Query(_fileId);
    if (opLineIndex.has_value()) {
        return opLineIndex.value();
    } else {
        auto opText = fileDB.Query(_fileId);
        if (opText.has_value()) {
            auto lineIndex = std::make_shared<LineIndex>();
            auto &text = *opText.value();
            lineIndex->Parse(text);
            lineIndexDB.Input(_fileId, lineIndex);
            return lineIndex;
        }
    }
    return nullptr;
}

bool VirtualFile::IsNull() const {
    return _fileId == 0;
}

//void VirtualFile::UpdateFile(lsp::Range range, std::string&& content)
//{
//	if (_luaFile == nullptr)
//	{
//		return UpdateFile(std::move(content));
//	}
//
//	std::string& text = _luaFile->GetSource();
//
//	auto startOffset = _luaFile->GetOffsetFromPosition(range.start.line, range.start.character);
//	auto endOffset = _luaFile->GetOffsetFromPosition(range.end.line, range.end.character);
//
//	auto oldSize = text.size();
//	auto newSize = oldSize + (content.size() - (endOffset - startOffset));
//	if (newSize > text.capacity())
//	{
//		auto suitableCapacity = newSize + 4096;
//		text.reserve(suitableCapacity);
//	}
//
//	if (newSize > oldSize)
//	{
//		text.resize(newSize);
//		std::copy_backward(text.begin() + endOffset, text.begin() + oldSize, text.end());
//		std::copy(content.begin(), content.end(), text.begin() + startOffset);
//	}
//	else
//	{
//		std::copy(text.begin() + endOffset, text.end(), text.begin() + startOffset + content.size());
//		if (content.size() > 0)
//		{
//			std::copy(content.begin(), content.end(), text.begin() + startOffset);
//		}
//		text.resize(newSize);
//	}
//
//	_luaFile->UpdateLineInfo(range.start.line);
//	_luaParser = nullptr;
//	++_version;
//}

//void VirtualFile::UpdateFile(std::vector<lsp::TextDocumentContentChangeEvent>& changeEvent)
//{
//	if (_luaFile == nullptr)
//	{
//		return;
//	}
//
//	std::string text;
//	int64_t totalSize = static_cast<int64_t>(_luaFile->GetSource().size());
//	std::vector<std::pair<TextRange, std::string>> textChanges;
//	for (auto& change : changeEvent)
//	{
//		if (!change.range.has_value())
//		{
//			return;
//		}
//		auto range = change.range.value();
//		auto& content = change.text;
//		auto startOffset = _luaFile->GetOffsetFromPosition(range.start.line, range.start.character);
//		auto endOffset = _luaFile->GetOffsetFromPosition(range.end.line, range.end.character);
//		textChanges.emplace_back(TextRange{startOffset, endOffset}, std::move(content));
//		totalSize += content.size() - (endOffset - startOffset);
//	}
//
//	std::stable_sort(textChanges.begin(), textChanges.end(), [](auto& x, auto& y)-> bool
//	{
//		return x.first.StartOffset < y.first.StartOffset;
//	});
//
//	if (totalSize > 0)
//	{
//		auto& source = _luaFile->GetSource();
//		text.reserve(totalSize);
//		std::size_t start = 0;
//		for (std::size_t index = 0; index != textChanges.size(); index++)
//		{
//			auto textRange = textChanges[index].first;
//			if (start < textRange.StartOffset)
//			{
//				text.append(source.begin() + start, source.begin() + textRange.StartOffset);
//			}
//
//			auto& content = textChanges[index].second;
//
//			text.append(content);
//
//			start = textChanges[index].first.EndOffset;
//		}
//
//		if (start < source.size())
//		{
//			text.append(source.begin() + start, source.end());
//		}
//	}
//
//	UpdateFile(std::move(text));
//}

std::string FilePath2FileName(std::string_view filePath) {
    if (filePath.empty()) {
        return "";
    }


    for (std::size_t index = filePath.size(); index > 0; index--) {
        char c = filePath[index - 1];
        if (c == '/' || c == '\\') {
            return std::string(filePath.substr(index));
        }
    }

    return std::string(filePath);
}

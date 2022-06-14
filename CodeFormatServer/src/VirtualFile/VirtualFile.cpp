#include "CodeFormatServer/VirtualFile/VirtualFile.h"
#include "Util/Url.h"

VirtualFile::VirtualFile(std::string_view fileUri)
	: _fileUri(fileUri),
	  _luaFile(nullptr),
	  _luaParser(nullptr),
	  _version(0)
{
}

void VirtualFile::UpdateFile(vscode::Range range, std::string&& content)
{
	if (_luaFile == nullptr)
	{
		return UpdateFile(std::move(content));
	}

	std::string& text = _luaFile->GetSource();

	auto startOffset = _luaFile->GetOffsetFromPosition(range.start.line, range.start.character);
	auto endOffset = _luaFile->GetOffsetFromPosition(range.end.line, range.end.character);

	auto oldSize = text.size();
	auto newSize = oldSize + (content.size() - (endOffset - startOffset));
	if (newSize > text.capacity())
	{
		auto suitableCapacity = newSize + 4096;
		text.reserve(suitableCapacity);
	}

	if (newSize > oldSize)
	{
		text.resize(newSize);
		std::copy_backward(text.begin() + endOffset, text.begin() + oldSize, text.end());
		std::copy(content.begin(), content.end(), text.begin() + startOffset);
	}
	else
	{
		std::copy(text.begin() + endOffset, text.end(), text.begin() + startOffset + content.size());
		if (content.size() > 0)
		{
			std::copy(content.begin(), content.end(), text.begin() + startOffset);
		}
		text.resize(newSize);
	}

	_luaFile->UpdateLineInfo(range.start.line);
	_luaParser = nullptr;
	++_version;
}

void VirtualFile::UpdateFile(std::vector<vscode::TextDocumentContentChangeEvent>& changeEvent)
{
	if (_luaFile == nullptr)
	{
		return;
	}

	std::string text;
	int64_t totalSize = static_cast<int64_t>(_luaFile->GetSource().size());
	std::vector<std::pair<TextRange, std::string>> textChanges;
	for (auto& change : changeEvent)
	{
		if (!change.range.has_value())
		{
			return;
		}
		auto range = change.range.value();
		auto& content = change.text;
		auto startOffset = _luaFile->GetOffsetFromPosition(range.start.line, range.start.character);
		auto endOffset = _luaFile->GetOffsetFromPosition(range.end.line, range.end.character);
		textChanges.emplace_back(TextRange{startOffset, endOffset}, std::move(content));
		totalSize += content.size() - (endOffset - startOffset);
	}

	std::stable_sort(textChanges.begin(), textChanges.end(), [](auto& x, auto& y)-> bool
	{
		return x.first.StartOffset < y.first.StartOffset;
	});

	if (totalSize > 0)
	{
		auto& source = _luaFile->GetSource();
		text.reserve(totalSize);
		std::size_t start = 0;
		for (std::size_t index = 0; index != textChanges.size(); index++)
		{
			auto textRange = textChanges[index].first;
			if (start < textRange.StartOffset)
			{
				text.append(source.begin() + start, source.begin() + textRange.StartOffset);
			}

			auto& content = textChanges[index].second;

			text.append(content);

			start = textChanges[index].first.EndOffset;
		}

		if (start < source.size())
		{
			text.append(source.begin() + start, source.end());
		}
	}

	UpdateFile(std::move(text));
}

void VirtualFile::UpdateFile(std::string&& text)
{
	_luaFile = std::make_shared<LuaFile>(std::filesystem::path(url::UrlToFilePath(_fileUri)).filename().string(),
	                                     std::move(text));
	++_version;
	MakeParser();
}

void VirtualFile::Reset()
{
}

std::shared_ptr<LuaParser> VirtualFile::GetLuaParser()
{
	if (!_luaParser)
	{
		MakeParser();
	}

	return _luaParser;
}

void VirtualFile::MakeParser()
{
	auto tokenParser = std::make_shared<LuaTokenParser>(_luaFile);
	_luaParser = std::make_shared<LuaParser>(tokenParser);
	_luaParser->BuildAstWithComment();
}

uint64_t VirtualFile::GetVersion()
{
	return _version;
}

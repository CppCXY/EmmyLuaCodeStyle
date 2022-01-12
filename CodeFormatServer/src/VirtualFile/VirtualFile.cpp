#include "CodeFormatServer/VirtualFile/VirtualFile.h"
#include "Util/Url.h"

VirtualFile::VirtualFile(std::string_view fileUri)
	: _fileUri(fileUri),
	  _luaFile(nullptr),
	  _luaParser(nullptr)
{
}

void VirtualFile::UpdateFile(vscode::Range range, std::string&& content)
{
	if (_luaFile == nullptr)
	{
		return UpdateFile(std::move(content));
	}

	std::string text;
	text.swap(_luaFile->GetSource());
	auto startOffset = _luaFile->GetOffsetFromPosition(range.start.line, range.start.character);
	auto endOffset = _luaFile->GetOffsetFromPosition(range.end.line, range.end.character);
	auto oldSize = text.size();
	auto newSize = oldSize + (content.size() - (endOffset - startOffset));
	if (newSize > text.capacity())
	{
		auto suitableCapacity = newSize + 4096;
		text.reserve(suitableCapacity);
	}

	if(newSize > oldSize)
	{
		text.resize(newSize);
		std::copy_backward(text.begin() + endOffset, text.begin() + oldSize, text.end());
		std::copy(content.begin(), content.end(), text.begin() + startOffset);
	}
	else
	{
		std::copy(text.begin() + endOffset, text.end(), text.begin() + startOffset + content.size());
		if(content.size() > 0)
		{
			std::copy(content.begin(), content.end(), text.begin() + startOffset);
		}
		text.resize(newSize);
	}

	_luaFile = std::make_shared<LuaFile>(std::filesystem::path(url::UrlToFilePath(_fileUri)).filename().string(),
	                                     std::move(text));
}

void VirtualFile::UpdateFile(std::string&& text)
{
	_luaFile = std::make_shared<LuaFile>(std::filesystem::path(url::UrlToFilePath(_fileUri)).filename().string(),
	                                     std::move(text));
}

void VirtualFile::Reset()
{
}

std::shared_ptr<LuaParser> VirtualFile::GetLuaParser()
{
	return _luaParser;
}

void VirtualFile::MakeParser()
{
	auto tokenParser = std::make_shared<LuaTokenParser>(_luaFile);
	tokenParser->Parse();
	_luaParser = std::make_shared<LuaParser>(tokenParser);
	_luaParser->BuildAstWithComment();
}

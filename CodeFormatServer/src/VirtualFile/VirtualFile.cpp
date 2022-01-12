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

	auto startOffset = _luaFile->GetOffsetFromPosition(range.start.line, range.start.character);
	auto endOffset = _luaFile->GetOffsetFromPosition(range.end.line, range.end.character);
	auto oldSize = _text.size();
	auto newSize = oldSize + (content.size() - (endOffset - startOffset));
	if (newSize > _text.capacity())
	{
		auto suitableCapacity = newSize + 4096;
		_text.reserve(suitableCapacity);
	}

	if(newSize > oldSize)
	{
		_text.resize(newSize);
		std::copy_backward(_text.begin() + endOffset, _text.begin() + oldSize, _text.end());
		std::copy(content.begin(), content.end(), _text.begin() + startOffset);
	}
	else
	{
		std::copy(_text.begin() + endOffset, _text.end(), _text.begin() + startOffset + content.size());
		if(content.size() > 0)
		{
			std::copy(content.begin(), content.end(), _text.begin() + startOffset);
		}
		_text.resize(newSize);
	}

	std::string fileText = _text;
	_luaFile = std::make_shared<LuaFile>(std::filesystem::path(url::UrlToFilePath(_fileUri)).filename().string(),
	                                     std::move(fileText));
	MakeParser();
}

void VirtualFile::UpdateFile(std::string&& text)
{
	_text = text;
	_luaFile = std::make_shared<LuaFile>(std::filesystem::path(url::UrlToFilePath(_fileUri)).filename().string(),
	                                     std::move(text));
	MakeParser();
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

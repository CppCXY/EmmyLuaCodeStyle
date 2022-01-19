#include "LuaCodeFormat.h"

#include "CodeService/LuaEditorConfig.h"
#include "CodeService/LuaFormatter.h"
#include "LuaParser/LuaParser.h"

LuaCodeFormat& LuaCodeFormat::GetInstance()
{
	static LuaCodeFormat instance;
	return instance;
}

LuaCodeFormat::LuaCodeFormat()
	: _defaultOptions(std::make_shared<LuaCodeStyleOptions>())
{
}

void LuaCodeFormat::UpdateCodeStyle(const std::string& workspaceUri, const std::string& configPath)
{
	auto editorconfig = LuaEditorConfig::LoadFromFile(configPath);

	if (editorconfig == nullptr)
	{
		return;
	}

	for (auto& pair : _editorConfigVector)
	{
		if (pair.first == workspaceUri)
		{
			pair.second = editorconfig;
			pair.second->SetWorkspace(workspaceUri);
			return;
		}
	}

	_editorConfigVector.push_back({
		workspaceUri, editorconfig
	});
	_editorConfigVector.back().second->SetWorkspace(workspaceUri);
}

void LuaCodeFormat::RemoveCodeStyle(const std::string& workspaceUri)
{
	for(auto it = _editorConfigVector.begin();it != _editorConfigVector.end();++it)
	{
		if(it->first == workspaceUri)
		{
			_editorConfigVector.erase(it);
			return;
		}
	}
}

std::string LuaCodeFormat::Reformat(const std::string& uri, std::string&& text)
{
	auto parser = LuaParser::LoadFromBuffer(std::move(text));
	parser->BuildAstWithComment();

	if (!parser->GetErrors().empty())
	{
		return "";
	}
	auto options = GetOptions(uri);
	LuaFormatter formatter(parser, *options);
	formatter.BuildFormattedElement();

	return formatter.GetFormattedText();
}

std::string LuaCodeFormat::RangeFormat(const std::string uri, LuaFormatRange range, std::string&& text)
{
	return "";
}

std::shared_ptr<LuaCodeStyleOptions> LuaCodeFormat::GetOptions(const std::string& uri)
{
	std::size_t matchLength = 0;
	std::shared_ptr<LuaCodeStyleOptions> options = _defaultOptions;
	for (auto it = _editorConfigVector.begin(); it != _editorConfigVector.end(); it++)
	{
		if (uri.starts_with(it->first) && it->first.size() > matchLength)
		{
			matchLength = it->first.size();
			options = it->second->Generate(uri);
		}
	}

	return options;
}

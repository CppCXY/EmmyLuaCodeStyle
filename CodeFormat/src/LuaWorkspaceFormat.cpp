#include "LuaWorkspaceFormat.h"
#include <iostream>
#include <regex>
#include <fstream>
#include "Util/format.h"
#include "Util/FileFinder.h"
#include "Util/StringUtil.h"
#include "Util/SymSpell/SymSpell.h"

LuaWorkspaceFormat::LuaWorkspaceFormat(std::string_view workspace)
	: _workspace(absolute(std::filesystem::path(workspace))),
	  _autoDetectConfig(false),
	  _defaultOptions(std::make_shared<LuaCodeStyleOptions>())
{
}

bool LuaWorkspaceFormat::SetConfigPath(std::string_view config)
{
	auto editorconfig = LuaEditorConfig::LoadFromFile(std::string(config));
	if (!editorconfig)
	{
		return false;
	}
	_editorConfigVector.push_back({
		_workspace.string(), editorconfig
	});
	_editorConfigVector.back().second->SetWorkspace(_workspace.string());
	return true;
}

void LuaWorkspaceFormat::AddIgnoresByFile(std::string_view gitIgnoreFile)
{
	std::fstream fin(std::string(gitIgnoreFile), std::ios::in);
	if (fin.is_open())
	{
		std::string line;
		while (!fin.eof())
		{
			std::getline(fin, line);
			auto newLine = StringUtil::TrimSpace(line);
			if (!StringUtil::StartWith(newLine, "#"))
			{
				_ignorePattern.push_back(std::string(newLine));
			}
		}
	}
}

void LuaWorkspaceFormat::AddIgnores(std::string_view pattern)
{
	_ignorePattern.push_back(std::string(pattern));
}

void LuaWorkspaceFormat::SetAutoDetectConfig(bool detect)
{
	_autoDetectConfig = detect;
}

void LuaWorkspaceFormat::SetKeyValues(std::map<std::string, std::string, std::less<>>& keyValues)
{
	if (!keyValues.empty())
	{
		LuaEditorConfig::ParseFromSection(_defaultOptions, keyValues);
	}
}

void LuaWorkspaceFormat::ReformatWorkspace()
{
	if (_autoDetectConfig)
	{
		CollectEditorconfig();
	}

	FileFinder finder(_workspace);
	finder.AddFindExtension(".lua");
	finder.AddFindExtension(".lua.txt");
	finder.AddIgnoreDirectory(".git");
	finder.AddIgnoreDirectory(".github");
	finder.AddIgnoreDirectory(".svn");
	finder.AddIgnoreDirectory(".idea");
	finder.AddIgnoreDirectory(".vs");
	finder.AddIgnoreDirectory(".vscode");
	for (auto pattern : _ignorePattern)
	{
		finder.AddignorePatterns(pattern);
	}

	auto files = finder.FindFiles();
	for (auto& file : files)
	{
		LuaFormat luaFormat;
		luaFormat.SetInputFile(file);
		luaFormat.SetOptions(GetOptions(file));
		luaFormat.SetOutputFile(file);
		if (luaFormat.Reformat())
		{
			std::cerr << Util::format("reformat {} succeed.", file) << std::endl;
		}
		else
		{
			std::cerr << Util::format("reformat {} fail.", file) << std::endl;
		}
	}
}

bool LuaWorkspaceFormat::CheckWorkspace()
{
	if (_autoDetectConfig)
	{
		CollectEditorconfig();
	}

	FileFinder finder(_workspace);
	finder.AddFindExtension(".lua");
	finder.AddFindExtension(".lua.txt");
	finder.AddIgnoreDirectory(".git");
	finder.AddIgnoreDirectory(".github");
	finder.AddIgnoreDirectory(".svn");
	finder.AddIgnoreDirectory(".idea");
	finder.AddIgnoreDirectory(".vs");
	finder.AddIgnoreDirectory(".vscode");
	for (auto pattern : _ignorePattern)
	{
		finder.AddignorePatterns(pattern);
	}

	auto files = finder.FindFiles();
	bool ret = true;
	auto workspaceString = _workspace.string();

	for (auto& file : files)
	{
		LuaFormat luaFormat;
		luaFormat.SetInputFile(file);
		luaFormat.SetOptions(GetOptions(file));
		ret &= luaFormat.Check(workspaceString, nullptr);
	}

	return ret;
}

void LuaWorkspaceFormat::CollectEditorconfig()
{
	FileFinder finder(_workspace);
	finder.AddFindFile(".editorconfig");
	finder.AddIgnoreDirectory(".git");
	finder.AddIgnoreDirectory(".github");
	finder.AddIgnoreDirectory(".svn");
	finder.AddIgnoreDirectory(".idea");
	finder.AddIgnoreDirectory(".vs");
	finder.AddIgnoreDirectory(".vscode");

	auto files = finder.FindFiles();

	for (auto& file : files)
	{
		auto editorconfig = LuaEditorConfig::LoadFromFile(file);

		std::filesystem::path filePath(file);

		if (editorconfig == nullptr)
		{
			continue;
		}

		_editorConfigVector.push_back({
			filePath.parent_path().string(), editorconfig
		});
		_editorConfigVector.back().second->SetWorkspace(filePath.parent_path().string());
	}
}

std::shared_ptr<LuaCodeStyleOptions> LuaWorkspaceFormat::GetOptions(std::string_view path)
{
	std::size_t matchLength = 0;
	std::shared_ptr<LuaCodeStyleOptions> options = _defaultOptions;
	for (auto it = _editorConfigVector.begin(); it != _editorConfigVector.end(); it++)
	{
		if (path.starts_with(it->first) && it->first.size() > matchLength)
		{
			matchLength = it->first.size();
			options = it->second->Generate(path);
		}
	}

	return options;
}

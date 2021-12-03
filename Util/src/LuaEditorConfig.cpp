#include "Util/LuaEditorConfig.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <regex>

#include "Util/StringUtil.h"

std::shared_ptr<LuaEditorConfig> LuaEditorConfig::LoadFromFile(const std::string& path)
{
	std::fstream fin(path, std::ios::in);
	if (fin.is_open())
	{
		std::stringstream s;
		s << fin.rdbuf();
		auto editorConfig = std::make_shared<LuaEditorConfig>(s.str());
		editorConfig->Parse();
		return editorConfig;
	}

	return nullptr;
}

LuaEditorConfig::LuaEditorConfig(std::string&& source)
	: _source(source)
{
}

void LuaEditorConfig::Parse()
{
	auto lines = StringUtil::Split(_source, "\n");

	bool luaSectionFounded = false;
	std::regex comment = std::regex(R"(^\s*(;|#))");
	std::regex luaSection = std::regex(R"(^\s*\[\s*([^\]]+)\s*\]\s*$)");
	std::regex valueRegex = std::regex(R"(^\s*([\w\d_]+)\s*=\s*(.+)$)");

	for (auto& lineView : lines)
	{
		std::string line(lineView);
		if (std::regex_search(line, comment))
		{
			continue;
		}

		std::smatch m;

		if (std::regex_search(line, m, luaSection))
		{
			auto section = m.str(1);
			luaSectionFounded = section.find(".lua") != std::string::npos;
			continue;
		}

		if (luaSectionFounded)
		{
			if (std::regex_search(line, m, valueRegex))
			{
				_configMap.insert({m.str(1), std::string(StringUtil::TrimSpace(m.str(2)))});
			}
		}
	}
}


bool LuaEditorConfig::Exist(std::string_view key)
{
	auto it = _configMap.find(key);
	return it != _configMap.end();
}

std::string LuaEditorConfig::Get(std::string_view key)
{
	auto it = _configMap.find(key);
	if(it != _configMap.end())
	{
		return it->second;
	}

	return "";
}

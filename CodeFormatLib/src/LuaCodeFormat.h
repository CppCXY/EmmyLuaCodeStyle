#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "CodeService/LuaCodeStyleOptions.h"
#include "CodeService/LuaDiagnosisInfo.h"
#include "CodeService/LuaEditorConfig.h"
#include "CodeService/LuaFormatRange.h"

class LuaCodeFormat
{
public:
	static LuaCodeFormat& GetInstance();

	LuaCodeFormat();

	void UpdateCodeStyle(const std::string& workspaceUri, const std::string& configPath);
	void RemoveCodeStyle(const std::string& workspaceUri);

	std::string Reformat(const std::string& uri, std::string&& text);

	std::string RangeFormat(const std::string& uri, LuaFormatRange& range, std::string&& text);

	std::pair<bool, std::vector<LuaDiagnosisInfo>> Diagnose(const std::string& uri, std::string&& text);

	std::shared_ptr<LuaCodeStyleOptions> GetOptions(const std::string& uri);
private:
	std::vector<std::pair<std::string, std::shared_ptr<LuaEditorConfig>>> _editorConfigVector;

	std::shared_ptr<LuaCodeStyleOptions> _defaultOptions;
};

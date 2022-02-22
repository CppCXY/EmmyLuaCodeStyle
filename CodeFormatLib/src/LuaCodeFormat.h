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
	using ConfigMap = std::map<std::string, std::string, std::less<>>;
	static LuaCodeFormat& GetInstance();

	LuaCodeFormat();

	void UpdateCodeStyle(const std::string& workspaceUri, const std::string& configPath);
	void RemoveCodeStyle(const std::string& workspaceUri);
	void SetDefaultCodeStyle(ConfigMap& configMap);

	std::string Reformat(const std::string& uri, std::string&& text,ConfigMap& configMap);

	std::string RangeFormat(const std::string& uri, LuaFormatRange& range, std::string&& text, ConfigMap& configMap);

	std::pair<bool, std::vector<LuaDiagnosisInfo>> Diagnose(const std::string& uri, std::string&& text);

	std::shared_ptr<LuaCodeStyleOptions> GetOptions(const std::string& uri);

	LuaCodeStyleOptions CalculateOptions(const std::string& uri, ConfigMap& configMap);
private:
	std::vector<std::pair<std::string, std::shared_ptr<LuaEditorConfig>>> _editorConfigVector;

	std::shared_ptr<LuaCodeStyleOptions> _defaultOptions;
};

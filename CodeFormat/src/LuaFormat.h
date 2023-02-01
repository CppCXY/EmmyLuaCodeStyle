#pragma once

#include <string>
#include <string_view>
#include <filesystem>
#include <cstring>
#include <optional>
#include "CodeService/Config/LuaStyle.h"
#include "LuaParser/File/LuaFile.h"
#include "LuaParser/Types/TextRange.h"
#include "Types.h"
#include "CodeService/Config/LuaDiagnosticStyle.h"

class LuaFormat {
public:
    LuaFormat();

    void SetWorkMode(WorkMode mode);

    void SetWorkspace(std::string_view workspace);

    bool SetInputFile(std::string_view input);

    bool ReadFromStdin();

    void SetOutputFile(std::string_view path);

    void AutoDetectConfig();

    void SetConfigPath(std::string_view configPath);

    void SetDefaultStyle(std::map<std::string, std::string, std::less<>> &keyValues);

    bool Reformat();

    bool Check();

    void AddIgnoresByFile(std::string_view ignoreFile);

    void AddIgnores(std::string_view pattern);

    void SupportNameStyleCheck();
private:
    std::optional<std::string> ReadFile(std::string_view path);

    LuaStyle GetStyle(std::string_view path);

    void DiagnosticInspection(std::string_view message, TextRange range, std::shared_ptr<LuaFile> file,
                              std::string_view path);

    bool ReformatSingleFile(std::string_view inputPath, std::string_view outPath, std::string &&sourceText);

    bool ReformatWorkspace();

    bool CheckSingleFile(std::string_view inputPath, std::string &&sourceText);

    bool CheckWorkspace();

    WorkMode _mode;
    std::string _inputPath;
    std::string _inputFileText;
    std::string _workspace;
    std::string _outPath;
    std::vector<LuaConfig> _configs;
    LuaStyle _defaultStyle;
    LuaDiagnosticStyle _diagnosticStyle;
    std::vector<std::string> _ignorePattern;
};

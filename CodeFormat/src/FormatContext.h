#pragma once

#include "CodeFormatCore/Config/LuaDiagnosticStyle.h"
#include "CodeFormatCore/Config/LuaStyle.h"
#include "Util.h"
#include <cstring>
#include <filesystem>
#include <string>
#include <string_view>


class FormatContext {
public:
    void SetWorkMode(WorkMode mode);

    void SetWorkspacePath(std::string_view workspace);

    void SetInputFilePath(std::string_view input);

    void SetOutputFilePath(std::string_view path);

    void SetConfigFilePath(std::string_view configPath);

    void SetDefaultStyleOptions(std::map<std::string, std::string, std::less<>> &keyValues);

    void AddIgnorePatternsFromFile(std::string_view ignoreFile);

    void AddIgnorePattern(std::string_view pattern);

    void SetFormatRange(bool isRangeLine, std::string_view rangeStr);

    void EnableAutoDetectConfig();

    void EnableNonStandardLuaSupport();

    void EnableNameStyleCheckSupport();

    void EnableCompleteOutputRangeSupport();

    void EnableJsonDump();

    [[nodiscard]] LuaStyle GetStyle(std::string_view path) const;

    [[nodiscard]] WorkMode GetWorkMode() const;

    [[nodiscard]] bool IsDumpJson() const;

    [[nodiscard]] bool IsNonStandardLua() const;

    [[nodiscard]] std::string_view GetOutputPath() const;

    [[nodiscard]] std::string_view GetWorkspace() const;

    [[nodiscard]] const std::vector<std::string> &GetIgnorePattern() const;

    [[nodiscard]] bool IsCompleteRangeFormat() const;

    [[nodiscard]] bool IsFormatLine() const;

    [[nodiscard]] std::string GetFormatRangeString() const;

    [[nodiscard]] std::string GetInputText() const;

    [[nodiscard]] std::string GetInputPath() const;

    [[nodiscard]] const LuaDiagnosticStyle &GetDiagnosticStyle() const;

private:
    WorkMode _workMode = WorkMode::File;
    std::string _inputPath;
    std::string _workspace;
    std::string _outPath;
    std::vector<LuaConfig> _configs;
    LuaStyle _defaultStyle;
    LuaDiagnosticStyle _diagnosticStyle;
    std::vector<std::string> _ignorePattern;
    // for range format
    bool _isCompleteOutputRangeFormat = false;
    bool _isRangeLine = false;
    std::string _rangeStr;
    bool _isSupportNonStandardLua = false;
    bool _dumpJson = false;
};

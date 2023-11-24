#include "FormatContext.h"

#include <Util/FileFinder.h>
#include <Util/StringUtil.h>
#include <fstream>
#include <iostream>

FormatContext::FormatContext() {
    _diagnosticStyle.name_style_check = false;
}

void FormatContext::SetWorkMode(WorkMode mode) {
    _workMode = mode;
}

void FormatContext::SetWorkspacePath(std::string_view workspace) {
    _workspace = workspace;
}

void FormatContext::SetInputFilePath(std::string_view input) {
    _inputPath = std::string(input);
}

bool IsSubRelative(const std::filesystem::path &path, const std::filesystem::path &base) {
    auto relative = std::filesystem::relative(path, base);
    auto relativeString = relative.string();
    if (relativeString.empty()) {
        return false;
    }
    if (relativeString == ".") {
        return true;
    }
    return !relativeString.starts_with(".");
}

void FormatContext::SetOutputFilePath(std::string_view path) {
    _outPath = std::string(path);
}

void FormatContext::SetConfigFilePath(std::string_view configPath) {
    if (!configPath.empty()) {
        auto &config = _configs.emplace_back("");
        config.Editorconfig = LuaEditorConfig::LoadFromFile(std::string(configPath));
        config.Editorconfig->Parse();
    }
}

void FormatContext::SetDefaultStyleOptions(std::map<std::string, std::string, std::less<>> &keyValues) {
    if (keyValues.empty()) {
        return;
    }
    _defaultStyle.Parse(keyValues);
}

void FormatContext::AddIgnorePatternsFromFile(std::string_view ignoreFile) {
    std::fstream fin(std::string(ignoreFile), std::ios::in);
    if (fin.is_open()) {
        std::string line;
        while (!fin.eof()) {
            std::getline(fin, line);
            if (auto newLine = string_util::TrimSpace(line); !string_util::StartWith(newLine, "#")) {
                _ignorePattern.emplace_back(newLine);
            }
        }
    }
}

void FormatContext::AddIgnorePattern(std::string_view pattern) {
    _ignorePattern.emplace_back(pattern);
}

void FormatContext::SetFormatRange(bool isRangeLine, std::string_view rangeStr) {
    _isRangeLine = isRangeLine;
    _rangeStr = std::string(rangeStr);
}

void FormatContext::EnableAutoDetectConfig() {
    switch (_workMode) {
        case WorkMode::File: {
            if (_workspace.empty()) {
                _workspace = std::filesystem::current_path().string();
            }
            std::filesystem::path workspace(_workspace);
            std::filesystem::path inputPath(_inputPath);
            if (!IsSubRelative(inputPath, workspace)) {
                return;
            }

            auto directory = absolute(inputPath.parent_path());
            while (IsSubRelative(directory, workspace)) {
                auto editorconfigPath = directory / ".editorconfig";
                if (std::filesystem::exists(editorconfigPath)) {
                    SetConfigFilePath(editorconfigPath.string());
                    return;
                }

                directory = directory.parent_path();
            }
        }
        case WorkMode::Stdin: {
            if (_workspace.empty() || _inputPath.empty()) {
                return;
            }
            std::filesystem::path workspace(_workspace);
            std::filesystem::path inputPath(_inputPath);
            if (!IsSubRelative(inputPath, workspace)) {
                return;
            }

            auto directory = absolute(inputPath.parent_path());
            while (IsSubRelative(directory, workspace)) {
                auto editorconfigPath = directory / ".editorconfig";
                if (std::filesystem::exists(editorconfigPath)) {
                    SetConfigFilePath(editorconfigPath.string());
                    return;
                }

                directory = directory.parent_path();
            }
        }
        default: {
            FileFinder finder(_workspace);
            finder.AddFindFile(".editorconfig");
            finder.AddIgnoreDirectory(".git");
            finder.AddIgnoreDirectory(".github");
            finder.AddIgnoreDirectory(".svn");
            finder.AddIgnoreDirectory(".idea");
            finder.AddIgnoreDirectory(".vs");
            finder.AddIgnoreDirectory(".vscode");

            auto files = finder.FindFiles();

            for (auto &file: files) {
                std::filesystem::path filePath(file);
                auto &config = _configs.emplace_back(filePath.parent_path().string());
                config.Editorconfig = LuaEditorConfig::LoadFromFile(std::string(file));
                config.Editorconfig->Parse();
            }
        }
    }
}

void FormatContext::EnableNonStandardLuaSupport() {
    _isSupportNonStandardLua = true;
}

void FormatContext::EnableNameStyleCheckSupport() {
    _diagnosticStyle.name_style_check = true;
}

void FormatContext::EnableCompleteOutputRangeSupport() {
    _isCompleteOutputRangeFormat = true;
}

void FormatContext::EnableJsonDump() {
    _dumpJson = true;
}

LuaStyle FormatContext::GetStyle(std::string_view path) const {
    std::shared_ptr<LuaEditorConfig> editorConfig = nullptr;
    std::size_t matchProcess = 0;
    for (auto &config: _configs) {
        if (string_util::StartWith(path, config.Workspace)) {
            if (config.Workspace.size() >= matchProcess) {
                matchProcess = config.Workspace.size();
                editorConfig = config.Editorconfig;
            }
        }
    }

    if (editorConfig) {
        return editorConfig->Generate(path);
    }
    return _defaultStyle;
}

WorkMode FormatContext::GetWorkMode() const {
    return _workMode;
}

bool FormatContext::IsDumpJson() const {
    return _dumpJson;
}

bool FormatContext::IsNonStandardLua() const {
    return _isSupportNonStandardLua;
}

std::string_view FormatContext::GetOutputPath() const {
    return _outPath;
}

std::string_view FormatContext::GetWorkspace() const {
    return _workspace;
}

const std::vector<std::string> &FormatContext::GetIgnorePattern() const {
    return _ignorePattern;
}

bool FormatContext::IsCompleteRangeFormat() const {
    return _isCompleteOutputRangeFormat;
}

bool FormatContext::IsFormatLine() const {
    return _isRangeLine;
}

std::string FormatContext::GetFormatRangeString() const {
    return _rangeStr;
}

std::string FormatContext::GetInputText() const {
    if (_workMode == WorkMode::Stdin) {
        std::stringstream buffer;
        buffer << std::cin.rdbuf();
        return std::move(buffer.str());
    } else {
        auto opText = ReadFile(_inputPath);
        if (opText.has_value()) {
            return std::move(opText.value());
        }
        return "";
    }
}

std::string FormatContext::GetInputPath() const {
    return _inputPath;
}

const LuaDiagnosticStyle &FormatContext::GetDiagnosticStyle() const {
    return _diagnosticStyle;
}

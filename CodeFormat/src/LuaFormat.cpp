#include "LuaFormat.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Util/format.h"
#include "CodeService/Config/LuaEditorConfig.h"
#include "Util/StringUtil.h"
#include "LuaParser/File/LuaFile.h"
#include "LuaParser/Types/TextRange.h"
#include "LuaParser/Lexer/LuaLexer.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "CodeService/Format/FormatBuilder.h"
#include "LuaParser/Parse/LuaParser.h"
#include "CodeService/Diagnostic/DiagnosticBuilder.h"
#include "Util/FileFinder.h"
#include "Util/Url.h"

LuaFormat::LuaFormat() {
    _diagnosticStyle.name_style_check = false;
}

void LuaFormat::SetWorkspace(std::string_view workspace) {
    _workspace = workspace;
}

bool LuaFormat::SetInputFile(std::string_view input) {
    _inputPath = std::string(input);
    auto opText = ReadFile(input);
    if (opText.has_value()) {
        _inputFileText = std::move(opText.value());
        return true;
    }
    return false;
}

bool LuaFormat::ReadFromStdin() {
    std::stringstream buffer;
    buffer << std::cin.rdbuf();
    _inputPath = "stdin";
    _inputFileText = std::move(buffer.str());
    return true;
}

void LuaFormat::SetOutputFile(std::string_view path) {
    _outPath = std::string(path);
}

bool IsSubRelative(std::filesystem::path &path, std::filesystem::path base) {
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

void LuaFormat::AutoDetectConfig() {
    if (_mode == WorkMode::File) {
        if (_inputPath == "stdin") {
            return;
        }

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
                SetConfigPath(editorconfigPath.string());
                return;
            }

            directory = directory.parent_path();
        }
    } else {
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

void LuaFormat::SetConfigPath(std::string_view configPath) {
    if (!configPath.empty()) {
        auto &config = _configs.emplace_back("");
        config.Editorconfig = LuaEditorConfig::LoadFromFile(std::string(configPath));
        config.Editorconfig->Parse();
    }
}

void LuaFormat::SetDefaultStyle(std::map<std::string, std::string, std::less<>> &keyValues) {
    _defaultStyle.ParseFromMap(keyValues);
}

bool LuaFormat::Reformat() {
    if (_mode == WorkMode::File) {
        return ReformatSingleFile(_inputPath, _outPath, std::move(_inputFileText));
    }
    return ReformatWorkspace();
}

bool LuaFormat::ReformatSingleFile(std::string_view inputPath, std::string_view outPath, std::string &&sourceText) {
    auto file = std::make_shared<LuaFile>(std::move(sourceText));
    LuaLexer luaLexer(file);
    luaLexer.Parse();

    LuaParser p(file, std::move(luaLexer.GetTokens()));
    p.Parse();

    if (p.HasError()) {
        return false;
    }

    LuaSyntaxTree t;
    t.BuildTree(p);

    LuaStyle style = GetStyle(inputPath);
    if (outPath.empty()) {
        style.detect_end_of_line = false;
        style.end_of_line = EndOfLine::LF;
    }

    FormatBuilder f(style);
    auto formattedText = f.GetFormatResult(t);

    if (!outPath.empty()) {
        std::fstream fout(std::string(outPath), std::ios::out | std::ios::binary);
        fout.write(formattedText.data(), formattedText.size());
        fout.close();
    } else {
        std::cout.write(formattedText.data(), formattedText.size());
    }
    return true;
}

bool LuaFormat::Check() {
    if (_mode == WorkMode::File) {
        if (_inputPath != "stdin") {
            std::cerr << util::format("Check {} ...", _inputPath) << std::endl;
        }

        if (CheckSingleFile(_inputPath, std::move(_inputFileText))) {
            std::cerr << util::format("Check {} ... ok", _inputPath) << std::endl;
            return true;
        }
        return false;
    }
    return CheckWorkspace();
}

void LuaFormat::DiagnosticInspection(std::string_view message, TextRange range, std::shared_ptr<LuaFile> file,
                                     std::string_view path) {
    auto startLine = file->GetLine(range.StartOffset);
    auto startChar = file->GetColumn(range.StartOffset);
    auto endLine = file->GetLine(range.EndOffset);
    auto endChar = file->GetColumn(range.EndOffset);
    std::cerr << util::format("\t{}({}:{} to {}:{}): {}", path, startLine + 1, startChar, endLine + 1, endChar,
                              message) << std::endl;
}

void LuaFormat::SetWorkMode(WorkMode mode) {
    _mode = mode;
}

std::optional<std::string> LuaFormat::ReadFile(std::string_view path) {
    std::string newPath(path);
#ifdef _WIN32
    std::fstream fin(newPath, std::ios::in | std::ios::binary);
#else
    std::fstream fin(newPath, std::ios::in);
#endif

    if (fin.is_open()) {
        std::stringstream s;
        s << fin.rdbuf();
        return std::move(s.str());
    }

    return std::nullopt;
}

LuaStyle LuaFormat::GetStyle(std::string_view path) {
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

bool LuaFormat::CheckSingleFile(std::string_view inputPath, std::string &&sourceText) {
    auto file = std::make_shared<LuaFile>(std::move(sourceText));
    LuaLexer luaLexer(file);
    luaLexer.Parse();

    LuaParser p(file, std::move(luaLexer.GetTokens()));
    p.Parse();

    if (_inputPath == "stdin") {
        _inputPath = "from stdin";
    }

    if (p.HasError()) {
        auto &errors = p.GetErrors();
        std::cout << util::format("Check {} ...\t{} error", inputPath, errors.size()) << std::endl;
        for (auto &error: errors) {
            DiagnosticInspection(error.ErrorMessage, error.ErrorRange, file, inputPath);
        }

        return false;
    }

    LuaSyntaxTree t;
    t.BuildTree(p);

    LuaStyle style = GetStyle(inputPath);
    DiagnosticBuilder diagnosticBuilder(style, _diagnosticStyle);
    diagnosticBuilder.CodeStyleCheck(t);
    diagnosticBuilder.NameStyleCheck(t);
    auto diagnostics = diagnosticBuilder.GetDiagnosticResults(t);
    if (!diagnostics.empty()) {
        std::cout << util::format("Check {}\t{} warning", inputPath, diagnostics.size()) << std::endl;

        for (auto &d: diagnostics) {
            DiagnosticInspection(d.Message, d.Range, file, inputPath);
        }

        return false;
    }
    return true;
}

bool LuaFormat::CheckWorkspace() {
    FileFinder finder(_workspace);
    finder.AddFindExtension(".lua");
    finder.AddFindExtension(".lua.txt");
    finder.AddIgnoreDirectory(".git");
    finder.AddIgnoreDirectory(".github");
    finder.AddIgnoreDirectory(".svn");
    finder.AddIgnoreDirectory(".idea");
    finder.AddIgnoreDirectory(".vs");
    finder.AddIgnoreDirectory(".vscode");
    for (auto pattern: _ignorePattern) {
        finder.AddignorePatterns(pattern);
    }

    auto files = finder.FindFiles();
    for (auto &filePath: files) {
        auto opText = ReadFile(filePath);
        std::string displayPath = filePath;
        if (!_workspace.empty())
        {
            displayPath = string_util::GetFileRelativePath(_workspace, filePath);
        }
        if (opText.has_value()) {
            if (CheckSingleFile(displayPath, std::move(opText.value()))) {
                std::cerr << util::format("Check {} ok.", displayPath) << std::endl;
            }
        } else {
            std::cerr << util::format("Can not read file {}", displayPath) << std::endl;
        }
    }
    return true;
}

void LuaFormat::AddIgnoresByFile(std::string_view ignoreConfigFile) {
    std::fstream fin(std::string(ignoreConfigFile), std::ios::in);
    if (fin.is_open()) {
        std::string line;
        while (!fin.eof()) {
            std::getline(fin, line);
            auto newLine = string_util::TrimSpace(line);
            if (!string_util::StartWith(newLine, "#")) {
                _ignorePattern.emplace_back(newLine);
            }
        }
    }
}

void LuaFormat::AddIgnores(std::string_view pattern) {
    _ignorePattern.emplace_back(pattern);
}

bool LuaFormat::ReformatWorkspace() {
    FileFinder finder(_workspace);
    finder.AddFindExtension(".lua");
    finder.AddFindExtension(".lua.txt");
    finder.AddIgnoreDirectory(".git");
    finder.AddIgnoreDirectory(".github");
    finder.AddIgnoreDirectory(".svn");
    finder.AddIgnoreDirectory(".idea");
    finder.AddIgnoreDirectory(".vs");
    finder.AddIgnoreDirectory(".vscode");
    for (auto pattern: _ignorePattern) {
        finder.AddignorePatterns(pattern);
    }

    auto files = finder.FindFiles();
    for (auto &filePath: files) {
        auto opText = ReadFile(filePath);
        std::string displayPath = filePath;
        if (!_workspace.empty())
        {
            displayPath = string_util::GetFileRelativePath(_workspace, filePath);
        }
        if (opText.has_value()) {
            if (ReformatSingleFile(displayPath, filePath, std::move(opText.value()))) {
                std::cerr << util::format("Reformat {} succeed.", displayPath) << std::endl;
            } else {
                std::cerr << util::format("Reformat {} fail.", displayPath) << std::endl;
            }
        } else {
            std::cerr << util::format("Can not read file {}", displayPath) << std::endl;
        }
    }
    return true;
}

void LuaFormat::SupportNameStyleCheck() {
    _diagnosticStyle.name_style_check = true;
}



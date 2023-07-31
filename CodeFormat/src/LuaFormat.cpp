#include "LuaFormat.h"
#include "CodeFormatCore/Config/LuaEditorConfig.h"
#include "CodeFormatCore/Diagnostic/DiagnosticBuilder.h"
#include "CodeFormatCore/Format/FormatBuilder.h"
#include "CodeFormatCore/RangeFormat/RangeFormatBuilder.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "LuaParser/File/LuaSource.h"
#include "LuaParser/Lexer/LuaLexer.h"
#include "LuaParser/Parse/LuaParser.h"
#include "LuaParser/Types/TextRange.h"
#include "Util/FileFinder.h"
#include "Util/StringUtil.h"
#include "Util/Url.h"
#include "Util/format.h"
#include <fstream>
#include <iostream>
#include <sstream>

LuaFormat::LuaFormat()
    : _mode(WorkMode::File),
      _isRangeLine(false),
      _isCompleteOutputRangeFormat(false),
      _isSupportNonStandardLua(false) {
    _diagnosticStyle.name_style_check = false;
}

void LuaFormat::SetWorkspace(std::string_view workspace) {
    _workspace = workspace;
}

void LuaFormat::SetInputFile(std::string_view input) {
    _inputPath = std::string(input);
}

bool LuaFormat::ReadFromInput() {
    auto opText = ReadFile(_inputPath);
    if (opText.has_value()) {
        _inputFileText = std::move(opText.value());
        return true;
    }
    return false;
}

bool LuaFormat::ReadFromStdin() {
    std::stringstream buffer;
    buffer << std::cin.rdbuf();
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
    } else if (_mode == WorkMode::Stdin) {
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
    if (keyValues.empty()) {
        return;
    }
    _defaultStyle.Parse(keyValues);
}

bool LuaFormat::Reformat() {
    switch (_mode) {
        case WorkMode::File:
        case WorkMode::Stdin: {
            return ReformatSingleFile(_inputPath, _outPath, std::move(_inputFileText));
        }
        case WorkMode::Workspace: {
            return ReformatWorkspace();
        }
    }
    return false;
}

bool LuaFormat::ReformatSingleFile(std::string_view inputPath, std::string_view outPath, std::string &&sourceText) {
    auto file = LuaSource::From(std::move(sourceText));
    LuaLexer luaLexer(file);
    if (_isSupportNonStandardLua) {
        luaLexer.SupportNonStandardSymbol();
    }

    luaLexer.Parse();

    LuaParser p(file, std::move(luaLexer.GetTokens()));
    p.Parse();

    if (p.HasError()) {
        std::cerr << "Exist Syntax Errors" << std::endl;
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


bool LuaFormat::RangeReformat() {
    auto file = LuaSource::From(std::move(_inputFileText));
    LuaLexer luaLexer(file);
    if (_isSupportNonStandardLua) {
        luaLexer.SupportNonStandardSymbol();
    }

    luaLexer.Parse();

    LuaParser p(file, std::move(luaLexer.GetTokens()));
    p.Parse();

    if (p.HasError()) {
        std::cerr << "Exist Syntax Errors" << std::endl;
        return false;
    }

    LuaSyntaxTree t;
    t.BuildTree(p);

    LuaStyle style = GetStyle(_inputPath);
    style.detect_end_of_line = false;
    style.end_of_line = EndOfLine::LF;
    FormatRange range;

    auto rangeVec = string_util::Split(_rangeStr, ":");
    if (rangeVec.size() != 2) {
        std::cerr << "Range param format error" << std::endl;
        return false;
    }
    auto start = std::stoull(std::string(rangeVec[0]));
    auto end = std::stoull(std::string(rangeVec[1]));
    if (_isRangeLine) {
        range.StartLine = start;
        range.EndLine = end;
        range.StartCol = 0;
        range.EndCol = 0;
    } else {
        range.StartLine = file->GetLine(start);
        range.EndLine = file->GetLine(end);
    }

    RangeFormatBuilder f(style, range);
    auto formattedText = f.GetFormatResult(t);
    auto replaceRange = f.GetReplaceRange();

    if (_isCompleteOutputRangeFormat) {
        start = file->GetOffset(replaceRange.StartLine, replaceRange.StartCol);
        end = file->GetOffset(replaceRange.EndLine + 1, 0);
        std::string source = std::string(file->GetSource());
        source.replace(start, end - start, formattedText);
        formattedText = source;
    }

    std::cout.write(formattedText.data(), formattedText.size());
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

void LuaFormat::DiagnosticInspection(std::string_view message, TextRange range, std::shared_ptr<LuaSource> file,
                                     std::string_view path) {
    auto startLine = file->GetLine(range.StartOffset);
    auto startChar = file->GetColumn(range.StartOffset);
    auto endLine = file->GetLine(range.GetEndOffset());
    auto endChar = file->GetColumn(range.GetEndOffset());
    std::cerr << util::format("\t{}({}:{} to {}:{}): {}", path, startLine + 1, startChar, endLine + 1, endChar,
                              message)
              << std::endl;
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
    auto file = std::make_shared<LuaSource>(std::move(sourceText));
    LuaLexer luaLexer(file);
    if (_isSupportNonStandardLua) {
        luaLexer.SupportNonStandardSymbol();
    }
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
        if (!_workspace.empty()) {
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
        if (!_workspace.empty()) {
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

void LuaFormat::SupportCompleteOutputRange() {
    _isCompleteOutputRangeFormat = true;
}

void LuaFormat::SetFormatRange(bool rangeLine, std::string_view rangeStr) {
    _isRangeLine = rangeLine;
    _rangeStr = std::string(rangeStr);
}

void LuaFormat::SupportNonStandardLua() {
    _isSupportNonStandardLua = true;
}

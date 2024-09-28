#include "LuaCheck.h"
#include "nlohmann/json.hpp"
#include <CodeFormatCore/Diagnostic/DiagnosticBuilder.h>
#include <LuaParser/File/LuaSource.h>
#include <LuaParser/Lexer/LuaLexer.h>
#include <LuaParser/Parse/LuaParser.h>
#include <Util/FileFinder.h>
#include <Util/format.h>
#include <iostream>

bool LuaCheck::Check(const FormatContext &context) {
    switch (context.GetWorkMode()) {
        case WorkMode::File:
        case WorkMode::Stdin: {
            auto inputPath = context.GetInputPath();
            if (CheckSingleFile(context, inputPath, std::move(context.GetInputText()), context.GetStyle(inputPath))) {
                std::cerr << util::format("Check {} ... ok", inputPath) << std::endl;
                return true;
            }
            break;
        }
        case WorkMode::Workspace: {
            return CheckWorkspace(context);
        }
    }
    return false;
}

void DiagnosticInspection(std::string_view message, TextRange range, LuaSource &file,
                          std::string_view path) {
    auto startLine = file.GetLine(range.StartOffset);
    auto startChar = file.GetColumn(range.StartOffset);
    auto endLine = file.GetLine(range.GetEndOffset());
    auto endChar = file.GetColumn(range.GetEndOffset());
    std::cerr << util::format("\t{}({}:{} to {}:{}): {}", path, startLine + 1, startChar, endLine + 1, endChar,
                              message)
              << std::endl;
}

nlohmann::json DiagnosticToJson(std::string_view message, TextRange range, LuaSource &file) {
    auto startLine = file.GetLine(range.StartOffset);
    auto startChar = file.GetColumn(range.StartOffset);
    auto endLine = file.GetLine(range.GetEndOffset());
    auto endChar = file.GetColumn(range.GetEndOffset());
    auto obj = nlohmann::json::object();
    obj["message"] = message;
    obj["range"] = {
            {"start", {{"line", startLine}, {"character", startChar}}},
            {"end",   {{"line", endLine}, {"character", endChar}}    }
    };
    return obj;
}

bool LuaCheck::CheckSingleFile(const FormatContext &context, std::string_view path, std::string &&sourceText, LuaStyle style) {
    auto file = std::make_shared<LuaSource>(std::move(sourceText));
    LuaLexer luaLexer(file);
    if (context.IsNonStandardLua()) {
        luaLexer.SupportNonStandardSymbol();
    }
    if (context.IsCLikeCommentsSupport()) {
        luaLexer.SupportCLikeComments();
    }

    luaLexer.Parse();

    LuaParser p(file, std::move(luaLexer.GetTokens()));
    p.Parse();

    if (p.HasError()) {
        auto &errors = p.GetErrors();
        std::cout << util::format("Check {} ...\t{} error", path, errors.size()) << std::endl;
        for (auto &error: errors) {
            DiagnosticInspection(error.ErrorMessage, error.ErrorRange, *file, path);
        }

        return false;
    }

    LuaSyntaxTree t;
    t.BuildTree(p);
    auto diagnosticStyle = context.GetDiagnosticStyle();
    DiagnosticBuilder diagnosticBuilder(style, diagnosticStyle);
    diagnosticBuilder.CodeStyleCheck(t);
    diagnosticBuilder.NameStyleCheck(t);

    auto diagnostics = diagnosticBuilder.GetDiagnosticResults(t);
    if (diagnostics.empty()) {
        return true;
    }

    if (context.IsDumpJson()) {
        std::cerr << util::format("Check {}\t{} warning", path, diagnostics.size()) << std::endl;

        auto arr = nlohmann::json::array();
        for (auto &d: diagnostics) {
            auto obj = DiagnosticToJson(d.Message, d.Range, *file);
            arr.push_back(obj);
        }
        std::cout << arr.dump() << std::endl;

        return false;
    } else {
        std::cerr << util::format("Check {}\t{} warning", path, diagnostics.size()) << std::endl;

        for (auto &d: diagnostics) {
            DiagnosticInspection(d.Message, d.Range, *file, path);
        }

        return false;
    }
}

bool LuaCheck::CheckWorkspace(const FormatContext &context) {
    auto workspace = context.GetWorkspace();
    bool diagnosis = false;
    FileFinder finder(workspace);
    finder.AddFindExtension(".lua");
    finder.AddFindExtension(".lua.txt");
    finder.AddIgnoreDirectory(".git");
    finder.AddIgnoreDirectory(".github");
    finder.AddIgnoreDirectory(".svn");
    finder.AddIgnoreDirectory(".idea");
    finder.AddIgnoreDirectory(".vs");
    finder.AddIgnoreDirectory(".vscode");
    for (const auto &pattern: context.GetIgnorePattern()) {
        finder.AddIgnorePatterns(pattern);
    }

    auto files = finder.FindFiles();
    for (auto &filePath: files) {
        auto opText = ReadFile(filePath);
        std::string relativePath = filePath;
        if (!workspace.empty()) {
            relativePath = string_util::GetFileRelativePath(workspace, filePath);
        }
        if (opText.has_value()) {
            auto style = context.GetStyle(relativePath);
            if (CheckSingleFile(context, relativePath, std::move(opText.value()), style)) {
                std::cerr << util::format("Check {} ok.", relativePath) << std::endl;
            } else {
                diagnosis = true;
            }
        } else {
            std::cerr << util::format("Can not read file {}", relativePath) << std::endl;
        }
    }
    return !diagnosis;
}

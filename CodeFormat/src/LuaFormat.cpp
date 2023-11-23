#include "LuaFormat.h"
#include "CodeFormatCore/Diagnostic/DiagnosticBuilder.h"
#include "CodeFormatCore/Format/FormatBuilder.h"
#include "CodeFormatCore/RangeFormat/RangeFormatBuilder.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "LuaParser/File/LuaSource.h"
#include "LuaParser/Lexer/LuaLexer.h"
#include "LuaParser/Parse/LuaParser.h"
#include "Util/FileFinder.h"
#include "Util/StringUtil.h"
#include "Util/Url.h"
#include "Util/format.h"
#include <fstream>
#include <iostream>
#include <sstream>

bool LuaFormat::Reformat(const FormatContext &context) {
    switch (context.GetWorkMode()) {
        case WorkMode::File:
        case WorkMode::Stdin: {
            auto outPath = context.GetOutputPath();
            auto text = context.GetInputText();
            auto inputPath = context.GetInputPath();
            auto style = context.GetStyle(inputPath);
            return ReformatSingleFile(context, outPath, std::move(text), style);
        }
        case WorkMode::Workspace: {
            return ReformatWorkspace(context);
        }
    }
    return false;
}

bool LuaFormat::ReformatSingleFile(const FormatContext &context, std::string_view outPath, std::string &&sourceText, LuaStyle style) {
    auto file = LuaSource::From(std::move(sourceText));
    LuaLexer luaLexer(file);
    if (context.IsNonStandardLua()) {
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

bool LuaFormat::RangeReformat(const FormatContext &context) {
    auto text = context.GetInputText();
    auto file = LuaSource::From(std::move(text));
    LuaLexer luaLexer(file);
    if (context.IsNonStandardLua()) {
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

    auto style = context.GetStyle(context.GetInputPath());
    style.detect_end_of_line = false;
    style.end_of_line = EndOfLine::LF;
    FormatRange range;

    auto rangeStr = context.GetFormatRangeString();
    auto rangeVec = string_util::Split(rangeStr, ":");
    if (rangeVec.size() != 2) {
        std::cerr << "Range param format error" << std::endl;
        return false;
    }
    auto start = std::stoull(std::string(rangeVec[0]));
    auto end = std::stoull(std::string(rangeVec[1]));
    if (context.IsFormatLine()) {
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

    if (context.IsCompleteRangeFormat()) {
        start = file->GetOffset(replaceRange.StartLine, replaceRange.StartCol);
        end = file->GetOffset(replaceRange.EndLine + 1, 0);
        std::string source = std::string(file->GetSource());
        source.replace(start, end - start, formattedText);
        formattedText = source;
    }

    std::cout.write(formattedText.data(), formattedText.size());
    return true;
}

bool LuaFormat::ReformatWorkspace(const FormatContext &context) {
    auto workspace = context.GetWorkspace();
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
            if (ReformatSingleFile(context, filePath, std::move(opText.value()), style)) {
                std::cerr << util::format("Reformat {} succeed.", relativePath) << std::endl;
            } else {
                std::cerr << util::format("Reformat {} fail.", relativePath) << std::endl;
            }
        } else {
            std::cerr << util::format("Can not read file {}", relativePath) << std::endl;
        }
    }
    return true;
}

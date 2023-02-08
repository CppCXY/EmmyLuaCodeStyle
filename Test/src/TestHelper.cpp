#include "TestHelper.h"
#include "CodeService/RangeFormat/RangeFormatBuilder.h"
#include <filesystem>

std::string TestHelper::ScriptBase = "";

LuaStyle TestHelper::DefaultStyle;

bool TestHelper::TestFormatted(std::string input, const std::string &shouldBe, LuaStyle &style) {
    auto file = std::make_shared<LuaFile>(std::move(input));
    LuaLexer luaLexer(file);
    luaLexer.Parse();

    LuaParser p(file, std::move(luaLexer.GetTokens()));
    p.Parse();

    LuaSyntaxTree t;
    t.BuildTree(p);

    FormatBuilder f(style);
    auto text = f.GetFormatResult(t);

    return string_util::TrimSpace(text) == string_util::TrimSpace(shouldBe);
}

bool TestHelper::TestRangeFormatted(std::size_t startLine, std::size_t endLine, std::string input,
                                    const std::string &shouldBe, LuaStyle &style) {
    auto file = std::make_shared<LuaFile>(std::move(input));
    LuaLexer luaLexer(file);
    luaLexer.Parse();

    LuaParser p(file, std::move(luaLexer.GetTokens()));
    p.Parse();

    LuaSyntaxTree t;
    t.BuildTree(p);

    FormatRange range;
    range.StartLine = startLine;
    range.EndLine = endLine;
    RangeFormatBuilder f(style, range);
    auto text = f.GetFormatResult(t);

    return string_util::TrimSpace(text) == string_util::TrimSpace(shouldBe);
}

void TestHelper::CollectLuaFile(std::filesystem::path directoryPath, std::vector<std::string> &paths,
                                std::filesystem::path &root) {
    if (!std::filesystem::exists(directoryPath)) {
        return;
    }

    for (auto &it: std::filesystem::directory_iterator(directoryPath)) {
        if (std::filesystem::is_directory(it.status())) {
            CollectLuaFile(it.path().string(), paths, root);
        } else if (it.path().extension() == ".lua") {
            paths.push_back(std::filesystem::relative(it.path(), root).string());
        }
    }
}

std::string TestHelper::ReadFile(const std::string &path) {
    std::filesystem::path base(ScriptBase);
    std::string newPath = (base / path).string();
#ifdef _WIN32
    std::fstream fin(newPath, std::ios::in | std::ios::binary);
#else
    std::fstream fin(newPath, std::ios::in);
#endif

    if (fin.is_open()) {
        std::stringstream s;
        s << fin.rdbuf();
        return s.str();
    }

    return "";
}

LuaParser TestHelper::GetParser(std::string input) {
    auto file = std::make_shared<LuaFile>(std::move(input));
    LuaLexer luaLexer(file);
    luaLexer.Parse();

    LuaParser p(file, std::move(luaLexer.GetTokens()));
    p.Parse();
    return std::move(p);
}

bool TestHelper::TestPattern(std::string_view pattern, std::string_view path) {
    EditorconfigPattern p;
    p.Compile(pattern);

    return p.Match(path);
}


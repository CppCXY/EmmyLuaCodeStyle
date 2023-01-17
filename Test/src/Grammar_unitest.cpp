#include <gtest/gtest.h>
#include "TestHelper.h"

std::string MakeErrors(LuaParser &p) {
    auto errors = p.GetErrors();
    std::string errorOut;
    for (auto &err: errors) {
        errorOut.append(
                util::format("msg: {}, at {}\n", err.ErrorMessage,
                             p.GetLuaFile()->GetLine(err.ErrorRange.StartOffset)));
    }
    return errorOut;
}

TEST(LuaGrammar, basic) {
    std::vector<std::string> paths;
    std::filesystem::path root(TestHelper::ScriptBase);
    std::filesystem::path dir = root / "grammar";
    TestHelper::CollectLuaFile(dir, paths, root);
    for (auto &filePath: paths) {
        auto text = TestHelper::ReadFile(filePath);
        auto p = TestHelper::GetParser(text);

        EXPECT_FALSE(p.HasError())
                            << util::format("error at {}\n", filePath)
                            << MakeErrors(p);
    }
}

TEST(LuaGrammar, extend) {
    std::string text = R"(
    local t = `123`
)";
    auto p = TestHelper::GetParser(text);

    EXPECT_FALSE(p.HasError()) << "extend grammar Interpolate strings test fail";
}

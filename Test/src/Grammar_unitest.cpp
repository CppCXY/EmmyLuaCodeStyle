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
    EXPECT_FALSE(TestHelper::GetParser("local t = `123`", true).HasError()) << "extend grammar Interpolate strings test fail";
    EXPECT_FALSE(TestHelper::GetParser(R"(
t+=123
t-=123
t/=123
t//=123
t*=123
t<<=123
t>>=123
t%=123
t|=123
t&=123
)", true).HasError()) << "extend grammar compound assignment operator test fail";
    EXPECT_FALSE(TestHelper::GetParser(R"(
if a && b || d && h then
    return a != 123 && ! false
end
while true do
    if a then
        continue
    end
end
)", true).HasError()) << "extend grammar C likely operator symbol test fail";
    EXPECT_FALSE(TestHelper::GetParser(R"(
local t = 123/*1233*/
)", true).HasError()) << "extend grammar c likely comment test fail";
    EXPECT_FALSE(TestHelper::GetParser(R"(
local t = {@abaa}
)", true).HasError()) << "extend grammar issue 119 test fail";
    EXPECT_FALSE(TestHelper::GetParser(R"(
local t = disableControl?.disableMovement
local t2 = ddd?:ffffff()
local t3 = ddd?["hello"]
)", true).HasError()) << "extend grammar nullable operator test fail";
}

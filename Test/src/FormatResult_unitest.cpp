#include <gtest/gtest.h>
#include "TestHelper.h"

LuaStyle style;

TEST(Format, localStatement) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = 123
)",
            R"(
local t = 123
)", style));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = function()
end
)",
            R"(
local t = function()
end
)", style));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = {
}
)",
            R"(
local t = {
}
)", style));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t =
{
    a = 123
}
)",
            R"(
local t =
{
    a = 123
}
)", style));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t, d ,e = aaaa, bbbb, ccccc
)",
            R"(
local t, d, e = aaaa, bbbb, ccccc
)", style));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t<const> = 1
)",
            R"(
local t <const> = 1
)", style));
}

TEST(Format, AssignStatement) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
t = 1
)",
            R"(
t = 1
)", style));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
t, d, e = 1, 2, 3
)",
            R"(
t, d, e = 1, 2, 3
)", style));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
t = function()
end
)",
            R"(
t = function()
end
)", style));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
t = { a, b, c, d }
)",
            R"(
t = { a, b, c, d }
)", style));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
t = {
a, b, c, d
}
)",
            R"(
t = {
    a, b, c, d
}
)", style));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
t =
{
a, b, c, d
}
)",
            R"(
t =
{
    a, b, c, d
}
)", style));
}

TEST(Format, IfStatement) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
if a then
end
)",
            R"(
if a then
end
)", style));
}

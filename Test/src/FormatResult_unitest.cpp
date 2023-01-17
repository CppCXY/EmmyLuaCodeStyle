#include <gtest/gtest.h>
#include "TestHelper.h"



TEST(Format, localStatement) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = 123
)",
            R"(
local t = 123
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = function()
end
)",
            R"(
local t = function()
end
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = {
}
)",
            R"(
local t = {
}
)"));
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
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t, d ,e = aaaa, bbbb, ccccc
)",
            R"(
local t, d, e = aaaa, bbbb, ccccc
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t<const> = 1
)",
            R"(
local t <const> = 1
)"));
}

TEST(Format, AssignStatement) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
t = 1
)",
            R"(
t = 1
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
t, d, e = 1, 2, 3
)",
            R"(
t, d, e = 1, 2, 3
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
t = function()
end
)",
            R"(
t = function()
end
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
t = { a, b, c, d }
)",
            R"(
t = { a, b, c, d }
)"));
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
)"));
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
)"));
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
)"));
}

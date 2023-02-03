#include <gtest/gtest.h>
#include "TestHelper.h"

TEST(RangeFormat, all_range) {
    EXPECT_TRUE(TestHelper::TestRangeFormatted(0, 100, R"(
local t = 123
function f()
    do return x end
    while true do return true end
    if a then return b end
    okkk = 123
end
)",R"(
local t = 123
function f()
    do return x end
    while true do return true end
    if a then return b end
    okkk = 123
end
)"));
}

TEST(RangeFormat, range) {
    EXPECT_TRUE(TestHelper::TestRangeFormatted(1, 1, R"(
local t  =  123


dddddddd   =     12313131
)",R"(
local t = 123
)"));
    EXPECT_TRUE(TestHelper::TestRangeFormatted(4, 4, R"(
local t  =  123


dddddddd   =     12313131
)",R"(
dddddddd = 12313131
)"));
    EXPECT_TRUE(TestHelper::TestRangeFormatted(2, 3, R"(
function f(aa,            b)
    local t  =  123
    local c = 1231
end
)",R"(
    local t = 123
    local c = 1231
)"));
}

TEST(RangeFormat, line_range) {
    EXPECT_TRUE(TestHelper::TestRangeFormatted(2, 2, R"(
local t = {
    a = 123
}
)",R"(
    a = 123
)"));
}
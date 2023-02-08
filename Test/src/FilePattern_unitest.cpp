#include <gtest/gtest.h>
#include "TestHelper.h"

TEST(FilePattern, simple) {
    EXPECT_TRUE(TestHelper::TestPattern("*", ".lua"));
    EXPECT_TRUE(TestHelper::TestPattern("*", ".txt"));

    EXPECT_TRUE(TestHelper::TestPattern("*.lua", ".lua"));
    EXPECT_TRUE(TestHelper::TestPattern("*.lua", "1.lua"));
    EXPECT_TRUE(TestHelper::TestPattern("*.lua", "哈哈.lua"));
    EXPECT_TRUE(TestHelper::TestPattern("*.lua", "1/1.lua"));
    EXPECT_TRUE(TestHelper::TestPattern("*.lua", "ogjogieoirg.lua"));
    EXPECT_TRUE(TestHelper::TestPattern("*.lua", "1\\1.lua"));

    EXPECT_TRUE(TestHelper::TestPattern("**.lua", ".lua"));
    EXPECT_TRUE(TestHelper::TestPattern("**.lua", "1.lua"));
    EXPECT_TRUE(TestHelper::TestPattern("**.lua", "哈哈.lua"));
    EXPECT_TRUE(TestHelper::TestPattern("**.lua", "1/1.lua"));
    EXPECT_TRUE(TestHelper::TestPattern("**.lua", "ogjogieoirg.lua"));
    EXPECT_TRUE(TestHelper::TestPattern("**.lua", "1\\1.lua"));

    EXPECT_TRUE(TestHelper::TestPattern("Content/*.lua", "Content/1.lua"));
    EXPECT_FALSE(TestHelper::TestPattern("Content/*.lua", "Content2/1.lua"));
    EXPECT_FALSE(TestHelper::TestPattern("Content/*.lua", "Content/1/1.lua"));
    EXPECT_FALSE(TestHelper::TestPattern("Content/*.lua", "1.lua"));
    EXPECT_TRUE(TestHelper::TestPattern("Content/aaa*.lua", "Content/aaa.lua"));
    EXPECT_TRUE(TestHelper::TestPattern("Content/aaa*.lua", "Content/aaaAAAAAA.lua"));
    EXPECT_FALSE(TestHelper::TestPattern("Content/aaa*.lua", "aaaAAAAAA.lua"));

    EXPECT_FALSE(TestHelper::TestPattern("Content/**.lua", "aaaAAAAAA.lua"));
    EXPECT_TRUE(TestHelper::TestPattern("Content/**.lua", "Content/1.lua"));
    EXPECT_TRUE(TestHelper::TestPattern("Content/**.lua", "Content/1/1.lua"));
    EXPECT_FALSE(TestHelper::TestPattern("Content/**.lua", "Content2/1/1.lua"));

    EXPECT_FALSE(TestHelper::TestPattern("?abc.lua", "abc.lua"));
    EXPECT_TRUE(TestHelper::TestPattern("?abc.lua", "Dabc.lua"));
    EXPECT_TRUE(TestHelper::TestPattern("?abc.lua", "/abc.lua"));

    EXPECT_TRUE(TestHelper::TestPattern("[abc]bc.lua", "abc.lua"));
    EXPECT_FALSE(TestHelper::TestPattern("[!a]bc.lua", "abc.lua"));
}



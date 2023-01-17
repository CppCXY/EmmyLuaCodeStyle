#include <gtest/gtest.h>
#include "TestHelper.h"

TEST(FormatPerformance, 10k_row) {
    auto text = TestHelper::ReadFile("performance/10k_row_code.lua");
    auto p = TestHelper::GetParser(text);
    LuaSyntaxTree t;
    t.BuildTree(p);

    FormatBuilder b(TestHelper::DefaultStyle);
    b.FormatAnalyze(t);
    auto formatted = b.GetFormatResult(t);
}
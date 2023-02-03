#include <gtest/gtest.h>
#include "TestHelper.h"

TEST(FormatByStyleOption, indent_style) {
    LuaStyle style;

    style.indent_style = IndentStyle::Space;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = function()
    local a = 131
    local b = function()
        local c = {
        }
    end
end
)",
            R"(
local t = function()
    local a = 131
    local b = function()
        local c = {
        }
    end
end
)", style));

    style.indent_style = IndentStyle::Tab;
    // next is tab
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = function()
    local a = 131
    local b = function()
        local c = {
        }
    end
end
)",
            R"(
local t = function()
	local a = 131
	local b = function()
		local c = {
		}
	end
end
)", style));

}

TEST(FormatByStyleOption, indent_size) {
    LuaStyle style;

    style.indent_size = 2;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = function()
    local a = 131
    local b = function()
        local c = {
        }
    end
end
)",
            R"(
local t = function()
  local a = 131
  local b = function()
    local c = {
    }
  end
end
)", style));

    style.indent_size = 3;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = function()
    local a = 131
    local b = function()
        local c = {
        }
    end
end
)",
            R"(
local t = function()
   local a = 131
   local b = function()
      local c = {
      }
   end
end
)", style));

    style.indent_size = 4;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = function()
    local a = 131
    local b = function()
        local c = {
        }
    end
end
)",
            R"(
local t = function()
    local a = 131
    local b = function()
        local c = {
        }
    end
end
)", style));
}

TEST(FormatByStyleOption, quote_style) {
    LuaStyle style;

    style.quote_style = QuoteStyle::None;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = "aaa" .. 'bbb' .. '\"\"' .. [[123]]
)",
            R"(
local t = "aaa" .. 'bbb' .. '\"\"' .. [[123]]
)", style));
    style.quote_style = QuoteStyle::Double;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = "aaa" .. 'bbb' .. [[123]] .. '""'
)",
            R"(
local t = "aaa" .. "bbb" .. [[123]] .. '""'
)", style));
    style.quote_style = QuoteStyle::Single;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = "aaa" .. 'bbb' .. '\"\"' .. [[123]] .. '""'
)",
            R"(
local t = 'aaa' .. 'bbb' .. '\"\"' .. [[123]] .. '""'
)", style));
}

TEST(FormatByStyleOption, continuation_indent) {
    LuaStyle style;

    style.continuation_indent = 4;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
if a
    or c then

end
table.insert(t, {
    aaa = 123
})
)",
            R"(
if a
    or c then

end
table.insert(t, {
    aaa = 123
})
)", style));
    style.continuation_indent = 8;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
if a
    or c then

end
table.insert(t, {
    aaa = 123
})
)",
            R"(
if a
        or c then

end
table.insert(t, {
        aaa = 123
})
)", style));
}

TEST(FormatByStyleOption, max_line_length) {
    LuaStyle style;

    style.max_line_length = 80;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function f(aaaaaaaaaaaaaaaaaaaaaaaa,bbbbbbbbbbbbbbbbbb,cccccccccccccccccccccccccccccc)

end
)",
            R"(
function f(aaaaaaaaaaaaaaaaaaaaaaaa, bbbbbbbbbbbbbbbbbb,
           cccccccccccccccccccccccccccccc)

end
)", style));
    style.max_line_length = 120;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function f(aaaaaaaaaaaaaaaaaaaaaaaa,bbbbbbbbbbbbbbbbbb,cccccccccccccccccccccccccccccc)

end
)",
            R"(
function f(aaaaaaaaaaaaaaaaaaaaaaaa, bbbbbbbbbbbbbbbbbb, cccccccccccccccccccccccccccccc)

end
)", style));
}

TEST(FormatByStyleOption, end_of_line_not_test_case) {
}

TEST(FormatByStyleOption, trailing_table_separator) {
    LuaStyle style;

    style.trailing_table_separator = TrailingTableSeparator::Keep;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = { aaa, bbb, ccc, }
)",
            R"(
local t = { aaa, bbb, ccc, }
)", style));
    style.trailing_table_separator = TrailingTableSeparator::Never;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = { aaa, bbb, ccc, }
)",
            R"(
local t = { aaa, bbb, ccc }
)", style));
    style.trailing_table_separator = TrailingTableSeparator::Always;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = { aaa, bbb, ccc }
)",
            R"(
local t = { aaa, bbb, ccc, }
)", style));
    style.trailing_table_separator = TrailingTableSeparator::Smart;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = { aaa, bbb, ccc, }
local d = {
    a = 123,
    b = 123
}
)",
            R"(
local t = { aaa, bbb, ccc }
local d = {
    a = 123,
    b = 123,
}
)", style));
}

TEST(FormatByStyleOption, call_arg_parentheses) {
    LuaStyle style;

    style.call_arg_parentheses = CallArgParentheses::Keep;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
p(123)
p "12313"
p("456")
)",
            R"(
p(123)
p "12313"
p("456")
)", style));
    style.call_arg_parentheses = CallArgParentheses::Remove;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
p(123)
p "12313"
p("456")
)",
            R"(
p(123)
p "12313"
p "456"
)", style));
    style.call_arg_parentheses = CallArgParentheses::RemoveStringOnly;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
p("456")
p({1231})
)",
            R"(
p "456"
p({ 1231 })
)", style));
    style.call_arg_parentheses = CallArgParentheses::RemoveTableOnly;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
p("456")
p({1231})
)",
            R"(
p("456")
p { 1231 }
)", style));
}

TEST(FormatByStyleOption, detect_end_of_line_no_test_case) {
}

TEST(FormatByStyleOption, insert_final_newline_no_test_case) {
}

TEST(FormatByStyleOption, space_around_table_field_list) {
    LuaStyle style;

    style.space_around_table_field_list = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = { aaa }
local t2 = {
}
local t3 = {}
)",
            R"(
local t = { aaa }
local t2 = {
}
local t3 = {}
)", style));
    style.space_around_table_field_list = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = { aaa }
local t2 = {
}
local t3 = {}
)",
            R"(
local t = {aaa}
local t2 = {
}
local t3 = {}
)", style));
}

TEST(FormatByStyleOption, space_before_attribute) {
    LuaStyle style;

    style.space_before_attribute = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t <const> = 123
local t2 <close> = {}
)",
            R"(
local t <const> = 123
local t2 <close> = {}
)", style));
    style.space_before_attribute = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t <const> = 123
local t2 <close> = {}
)",
            R"(
local t<const> = 123
local t2<close> = {}
)", style));
}

TEST(FormatByStyleOption, space_before_function_open_parenthesis) {
    LuaStyle style;

    style.space_before_function_open_parenthesis = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function f() end
)",
            R"(
function f ()
end
)", style));
    style.space_before_function_open_parenthesis = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function f() end
)",
            R"(
function f()
end
)", style));
}

TEST(FormatByStyleOption, space_before_function_call_open_parenthesis) {
    LuaStyle style;

    style.space_before_function_call_open_parenthesis = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
print(1, 2, 3)
)",
            R"(
print (1, 2, 3)
)", style));
    style.space_before_function_call_open_parenthesis = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
print(1, 2, 3)
)",
            R"(
print(1, 2, 3)
)", style));
}

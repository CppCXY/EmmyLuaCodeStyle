#include "TestHelper.h"
#include <gtest/gtest.h>

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
)",
            style));

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
)",
            style));
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
)",
            style));

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
)",
            style));

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
)",
            style));
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
)",
            style));
    style.quote_style = QuoteStyle::Double;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = "aaa" .. 'bbb' .. [[123]] .. '""'
)",
            R"(
local t = "aaa" .. "bbb" .. [[123]] .. '""'
)",
            style));
    style.quote_style = QuoteStyle::Single;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = "aaa" .. 'bbb' .. '\"\"' .. [[123]] .. '""'
)",
            R"(
local t = 'aaa' .. 'bbb' .. '\"\"' .. [[123]] .. '""'
)",
            style));
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
)",
            style));
    style.continuation_indent = 8;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
if a
    or c then

end
)",
            R"(
if a
        or c then

end
)",
            style));
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
)",
            style));
    style.max_line_length = 120;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function f(aaaaaaaaaaaaaaaaaaaaaaaa,bbbbbbbbbbbbbbbbbb,cccccccccccccccccccccccccccccc)

end
)",
            R"(
function f(aaaaaaaaaaaaaaaaaaaaaaaa, bbbbbbbbbbbbbbbbbb, cccccccccccccccccccccccccccccc)

end
)",
            style));
}

TEST(FormatByStyleOption, end_of_line_not_test_case) {
}

TEST(FormatByStyleOption, table_separator_style) {
    LuaStyle style;

    style.table_separator_style = TableSeparatorStyle::None;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = { aaa, bbb, ccc; }
)",
            R"(
local t = { aaa, bbb, ccc; }
)",
            style));
    style.table_separator_style = TableSeparatorStyle::Comma;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = { aaa, bbb, ccc; }
)",
            R"(
local t = { aaa, bbb, ccc, }
)",
            style));
    style.table_separator_style = TableSeparatorStyle::Semicolon;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = { aaa, bbb, ccc; }
)",
            R"(
local t = { aaa; bbb; ccc; }
)",
            style));
    style.table_separator_style = TableSeparatorStyle::OnlyKVColon;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = { aaa, bbb, ccc; }
local d = {
    d = 123,
    k = 456,
    c,
    e
}
)",
            R"(
local t = { aaa, bbb, ccc, }
local d = {
    d = 123;
    k = 456;
    c,
    e
}
)",
            style));
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
)",
            style));
    style.trailing_table_separator = TrailingTableSeparator::Never;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = { aaa, bbb, ccc, }
)",
            R"(
local t = { aaa, bbb, ccc }
)",
            style));
    style.trailing_table_separator = TrailingTableSeparator::Always;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = { aaa, bbb, ccc }
)",
            R"(
local t = { aaa, bbb, ccc, }
)",
            style));
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
)",
            style));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = { function ()
    local t = 123
end }

local t = { {
    okokok = 123
} }
)",
            R"(
local t = { function()
    local t = 123
end }

local t = { {
    okokok = 123,
} }
)",
            style));
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
)",
            style));
    style.call_arg_parentheses = CallArgParentheses::Remove;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
p(123)
p "12313"
p("456")
local t = {
    a({
        a = 123
    }),
    d = 123
}
)",
            R"(
p(123)
p "12313"
p "456"
local t = {
    a {
        a = 123
    },
    d = 123
}
)",
            style));
    style.call_arg_parentheses = CallArgParentheses::RemoveStringOnly;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
p("456")
p({1231})
)",
            R"(
p "456"
p({ 1231 })
)",
            style));
    style.call_arg_parentheses = CallArgParentheses::RemoveTableOnly;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
p("456")
p({1231})
)",
            R"(
p("456")
p { 1231 }
)",
            style));
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
)",
            style));
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
)",
            style));
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
)",
            style));
    style.space_before_attribute = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t <const> = 123
local t2 <close> = {}
)",
            R"(
local t<const> = 123
local t2<close> = {}
)",
            style));
}

TEST(FormatByStyleOption, space_before_function_open_parenthesis) {
    LuaStyle style;

    style.space_before_function_open_parenthesis = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function f() end
)",
            R"(
function f () end
)",
            style));
    style.space_before_function_open_parenthesis = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function f() end
)",
            R"(
function f() end
)",
            style));
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
)",
            style));
    style.space_before_function_call_open_parenthesis = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
print(1, 2, 3)
)",
            R"(
print(1, 2, 3)
)",
            style));
}

TEST(FormatByStyleOption, space_before_closure_open_parenthesis) {
    LuaStyle style;

    style.space_before_closure_open_parenthesis = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local f = function ()
end
)",
            R"(
local f = function ()
end
)",
            style));
    style.space_before_closure_open_parenthesis = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local f = function ()
end
)",
            R"(
local f = function()
end
)",
            style));
}

TEST(FormatByStyleOption, space_before_function_call_single_arg) {
    LuaStyle style;

    style.space_before_function_call_single_arg = FunctionSingleArgSpace::Always;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local f = p { a = 123 }
p "aaa"
)",
            R"(
local f = p { a = 123 }
p "aaa"
)",
            style));
    style.space_before_function_call_single_arg = FunctionSingleArgSpace::OnlyString;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local f = p { a = 123 }
p "aaa"
)",
            R"(
local f = p{ a = 123 }
p "aaa"
)",
            style));
    style.space_before_function_call_single_arg = FunctionSingleArgSpace::OnlyTable;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local f = p { a = 123 }
p "aaa"
)",
            R"(
local f = p { a = 123 }
p"aaa"
)",
            style));
    style.space_before_function_call_single_arg = FunctionSingleArgSpace::None;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local f = p { a = 123 }
p "aaa"
)",
            R"(
local f = p{ a = 123 }
p"aaa"
)",
            style));
}

TEST(FormatByStyleOption, space_before_open_square_bracket) {
    LuaStyle style;

    style.space_before_open_square_bracket = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
t [123] = 123
)",
            R"(
t [123] = 123
)",
            style));
    style.space_before_open_square_bracket = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
t [123] = 123
)",
            R"(
t[123] = 123
)",
            style));
}

TEST(FormatByStyleOption, space_inside_function_call_parentheses) {
    LuaStyle style;

    style.space_inside_function_call_parentheses = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
p(1,2,3)
p()
)",
            R"(
p( 1, 2, 3 )
p()
)",
            style));
    style.space_inside_function_call_parentheses = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
p(1,2,3)
p()
)",
            R"(
p(1, 2, 3)
p()
)",
            style));
}

TEST(FormatByStyleOption, space_inside_function_param_list_parentheses) {
    LuaStyle style;

    style.space_inside_function_param_list_parentheses = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function f()
end
function f2(a,b,c)
end
)",
            R"(
function f()
end

function f2( a, b, c )
end
)",
            style));
    style.space_inside_function_param_list_parentheses = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function f()
end

function f2(a,b,c)
end
)",
            R"(
function f()
end

function f2(a, b, c)
end
)",
            style));
}

TEST(FormatByStyleOption, space_inside_square_brackets) {
    LuaStyle style;

    style.space_inside_square_brackets = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = { [1] = 123 }
t[1] = 456
)",
            R"(
local t = { [ 1 ] = 123 }
t[ 1 ] = 456
)",
            style));
    style.space_inside_square_brackets = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = { [1] = 123 }
t[1] = 456
)",
            R"(
local t = { [1] = 123 }
t[1] = 456
)",
            style));
}

TEST(FormatByStyleOption, space_around_table_append_operator) {
    LuaStyle style;

    style.space_around_table_append_operator = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
t[#t+1] = 123
)",
            R"(
t[#t+1] = 123
)",
            style));
    style.space_around_table_append_operator = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
t[#t+1] = 123
)",
            R"(
t[#t + 1] = 123
)",
            style));
}

TEST(FormatByStyleOption, ignore_spaces_inside_function_call) {
    LuaStyle style;

    style.ignore_spaces_inside_function_call = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
map('k',      123,      456)
)",
            R"(
map('k',      123,      456)
)",
            style));
    style.ignore_spaces_inside_function_call = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
map('k',      123,      456)
)",
            R"(
map('k', 123, 456)
)",
            style));
}

TEST(FormatByStyleOption, space_before_inline_comment) {
    LuaStyle style;

    style.space_before_inline_comment = 1;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = 123 --hello
)",
            R"(
local t = 123 --hello
)",
            style));
    style.space_before_inline_comment = 2;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = 123 --hello
)",
            R"(
local t = 123  --hello
)",
            style));
}

TEST(FormatByStyleOption, space_around_math_operator) {
    LuaStyle style;

    style.space_around_math_operator = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = 123 + (456^7) / 4 * 5 - 9
)",
            R"(
local t = 123 + (456 ^ 7) / 4 * 5 - 9
)",
            style));
    style.space_around_math_operator = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = 123 + (456^7) / 4 * 5 - 9
)",
            R"(
local t = 123+(456^7)/4*5-9
)",
            style));
}

TEST(FormatByStyleOption, space_after_comma) {
    LuaStyle style;

    style.space_after_comma = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = 1,2,3
p(4,5,6)
)",
            R"(
local t = 1, 2, 3
p(4, 5, 6)
)",
            style));
    style.space_after_comma = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = 1,2,3
p(4,5,6)
)",
            R"(
local t = 1,2,3
p(4,5,6)
)",
            style));
}

TEST(FormatByStyleOption, space_after_comma_in_for_statement) {
    LuaStyle style;

    style.space_after_comma_in_for_statement = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
for i,j,k in pairs(a) do
end

for i = 1, 2, 3 do
end
)",
            R"(
for i, j, k in pairs(a) do
end

for i = 1, 2, 3 do
end
)",
            style));
    style.space_after_comma_in_for_statement = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
for i,j,k in pairs(a) do
end

for i = 1, 2, 3 do
end
)",
            R"(
for i,j,k in pairs(a) do
end

for i = 1,2,3 do
end
)",
            style));
}

TEST(FormatByStyleOption, space_around_concat_operator) {
    LuaStyle style;

    style.space_around_concat_operator = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = "123" .. 1
)",
            R"(
local t = "123" .. 1
)",
            style));
    style.space_around_concat_operator = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = "123" .. 1
)",
            R"(
local t = "123"..1
)",
            style));
}

TEST(FormatByStyleOption, align_call_args) {
    LuaStyle style;

    style.align_call_args = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
cpdd(1,
2,
3)
)",
            R"(
cpdd(1,
    2,
    3)
)",
            style));
    style.align_call_args = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
cpdd(1,
    2,
    3)
)",
            R"(
cpdd(1,
     2,
     3)
)",
            style));
}

TEST(FormatByStyleOption, align_function_params) {
    LuaStyle style;

    style.align_function_params = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function f(aaa,
    bbb,
    ccc)
end
)",
            R"(
function f(aaa,
    bbb,
    ccc)
end
)",
            style));
    style.align_function_params = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function f(aaa,
    bbb,
    ccc)
end
)",
            R"(
function f(aaa,
           bbb,
           ccc)
end
)",
            style));
}

TEST(FormatByStyleOption, align_continuous_assign_statement) {
    LuaStyle style;

    style.align_continuous_assign_statement = ContinuousAlign::None;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t  = 123
local dddd = 456
)",
            R"(
local t = 123
local dddd = 456
)",
            style));
    style.align_continuous_assign_statement = ContinuousAlign::WhenExtraSpace;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t  = 123
local dddd = 456
)",
            R"(
local t    = 123
local dddd = 456
)",
            style));
    style.align_continuous_assign_statement = ContinuousAlign::Always;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = 123
local dddd = 456
)",
            R"(
local t    = 123
local dddd = 456
)",
            style));
}

TEST(FormatByStyleOption, align_continuous_rect_table_field) {
    LuaStyle style;

    style.align_continuous_rect_table_field = ContinuousAlign::None;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = {
    aaa  = 123,
    bbbbbbbb = 456
}
)",
            R"(
local t = {
    aaa = 123,
    bbbbbbbb = 456
}
)",
            style));
    style.align_continuous_rect_table_field = ContinuousAlign::WhenExtraSpace;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = {
    aaa  = 123,
    bbbbbbbb = 456
}
)",
            R"(
local t = {
    aaa      = 123,
    bbbbbbbb = 456
}
)",
            style));
    style.align_continuous_rect_table_field = ContinuousAlign::Always;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = {
    aaa = 123,
    bbbbbbbb = 456
}
)",
            R"(
local t = {
    aaa      = 123,
    bbbbbbbb = 456
}
)",
            style));
}

TEST(FormatByStyleOption, align_if_branch) {
    LuaStyle style;

    style.align_if_branch = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
if source.type ~= 'getglobal'
    and source.type ~= 'getlocal' then
    return
end
if t == 1 then
elseif b == 456 then
end
if c == 1
    and d == 3 then
end
)",
            R"(
if source.type ~= 'getglobal'
    and source.type ~= 'getlocal' then
    return
end
if t == 1 then
elseif b == 456 then
end
if c == 1
    and d == 3 then
end
)",
            style));
    style.align_if_branch = true;
    style.never_indent_before_if_condition = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
if source.type ~= 'getglobal'
    and source.type ~= 'getlocal' then
    return
end
if t == 1 then
elseif b == 456 then
end
if c == 1
    and d == 3 then
end
)",
            R"(
if  source.type ~= 'getglobal'
and source.type ~= 'getlocal' then
    return
end
if t == 1 then
elseif b == 456 then
end
if  c == 1
and d == 3 then
end
)",
            style));
}

TEST(FormatByStyleOption, align_array_table) {
    LuaStyle style;

    style.align_array_table = AlignArrayTable::None;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = {
    { "fwfwf", njqoifjiowiof, 121313 },
    { "fwfw", fjwofw, wngjwoigw },
    { a, b, c, d },
    { 1, 2, 3 },
}
)",
            R"(
local t = {
    { "fwfwf", njqoifjiowiof, 121313 },
    { "fwfw", fjwofw, wngjwoigw },
    { a, b, c, d },
    { 1, 2, 3 },
}
)",
            style));
    style.align_array_table = AlignArrayTable::Normal;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = {
    { "fwfwf", njqoifjiowiof, 121313 },
    { "fwfw", fjwofw, wngjwoigw },
    { a, b, c, d },
    { 1, 2, 3 },
}
)",
            R"(
local t = {
    { "fwfwf", njqoifjiowiof, 121313 },
    { "fwfw",  fjwofw,        wngjwoigw },
    { a,       b,             c,        d },
    { 1,       2,             3 },
}
)",
            style));
    style.align_array_table = AlignArrayTable::ContainCurly;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = {
    { "fwfwf", njqoifjiowiof, 121313 },
    { "fwfw", fjwofw, wngjwoigw },
    { a, b, c, d },
    { 1, 2, 3 },
}
)",
            R"(
local t = {
    { "fwfwf", njqoifjiowiof, 121313      },
    { "fwfw",  fjwofw,        wngjwoigw   },
    { a,       b,             c,        d },
    { 1,       2,             3           },
}
)",
            style));
}

TEST(FormatByStyleOption, never_indent_before_if_condition) {
    LuaStyle style;

    style.never_indent_before_if_condition = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
if a
and b then
end
)",
            R"(
if a
    and b then
end
)",
            style));
    style.never_indent_before_if_condition = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
if a
and b then
end
)",
            R"(
if a
and b then
end
)",
            style));
}

TEST(FormatByStyleOption, never_indent_comment_on_if_branch) {
    LuaStyle style;

    style.never_indent_comment_on_if_branch = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
if a then
    local t = 13
    --hello
elseif b then
end
)",
            R"(
if a then
    local t = 13
    --hello
elseif b then
end
)",
            style));
    style.never_indent_comment_on_if_branch = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
if a then
    local t = 13
    --hello
elseif b then
end
)",
            R"(
if a then
    local t = 13
--hello
elseif b then
end
)",
            style));
}

TEST(FormatByStyleOption, break_all_list_when_line_exceed) {
    LuaStyle style;

    style.break_all_list_when_line_exceed = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = { aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa, bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb, cccccccccccccccccccccccccccccccccccccc, eeeeeeeeeeeeeeee }
)",
            R"(
local t = { aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa, bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb,
    cccccccccccccccccccccccccccccccccccccc, eeeeeeeeeeeeeeee }
)",
            style));
    style.break_all_list_when_line_exceed = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = { aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa, bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb, cccccccccccccccccccccccccccccccccccccc, eeeeeeeeeeeeeeee }
)",
            R"(
local t = {
    aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa,
    bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb,
    cccccccccccccccccccccccccccccccccccccc,
    eeeeeeeeeeeeeeee
}
)",
            style));
}

TEST(FormatByStyleOption, auto_collapse_lines) {
    LuaStyle style;

    style.auto_collapse_lines = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = {
    aaaa,
    bbbbbbb,
    dddd = 123,
    hhihi = 123
}
)",
            R"(
local t = {
    aaaa,
    bbbbbbb,
    dddd = 123,
    hhihi = 123
}
)",
            style));
    style.auto_collapse_lines = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = {
    aaaa,
    bbbbbbb,
    dddd = 123,
    hhihi = 123
}
)",
            R"(
local t = { aaaa, bbbbbbb, dddd = 123, hhihi = 123 }
)",
            style));
}

TEST(FormatByStyleOption, ignore_space_after_colon) {
    LuaStyle style;

    style.ignore_space_after_colon = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = switch(a)
    : case(function()
    end)
)",
            R"(
local t = switch(a)
    :case(function()
    end)
)",
            style));
    style.ignore_space_after_colon = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = switch(a)
    : case(function()
    end)
)",
            R"(
local t = switch(a)
    : case(function()
    end)
)",
            style));
}

TEST(FormatByStyleOption, remove_call_expression_list_finish_comma) {
    LuaStyle style;

    style.remove_call_expression_list_finish_comma = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
p(1,2,3,)
)",
            R"(
p(1, 2, 3,)
)",
            style));
    style.remove_call_expression_list_finish_comma = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
p(1,2,3,)
)",
            R"(
p(1, 2, 3)
)",
            style));
}

TEST(FormatByStyleOption, align_continuous_inline_comment) {
    LuaStyle style;

    style.align_continuous_inline_comment = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function f() --hello world
    local t = 132 --1313
end

local t = {
    { 11111, 2,   3333333333333 }, --hello
    { 123,   345, 46 }, --yes
    { 1,     2,   3 }, --hh
}

local t = {
    aa = 1, -- e1
    bbaa = 2, --bb
}
)",
            R"(
function f() --hello world
    local t = 132 --1313
end

local t = {
    { 11111, 2,   3333333333333 }, --hello
    { 123,   345, 46 }, --yes
    { 1,     2,   3 }, --hh
}

local t = {
    aa = 1, -- e1
    bbaa = 2, --bb
}
)",
            style));
    style.align_continuous_inline_comment = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function f() --hello world
    local t = 132 --1313
end

local t = {
    { 11111, 2,   3333333333333 }, --hello
    { 123,   345, 46 }, --yes
    { 1,     2,   3 }, --hh
}

local t = {
    aa = 1, -- e1
    bbaa = 2, --bb
}
)",
            R"(
function f()      --hello world
    local t = 132 --1313
end

local t = {
    { 11111, 2,   3333333333333 }, --hello
    { 123,   345, 46 },            --yes
    { 1,     2,   3 },             --hh
}

local t = {
    aa = 1,   -- e1
    bbaa = 2, --bb
}
)",
            style));
}

TEST(FormatByStyleOption, align_chain_expr) {
    LuaStyle style;

    style.align_chain_expr = AlignChainExpr::Always;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
return aaaaaaaaaaaa.wfgoiwjofjw()
     .afjoajofjw + 123131 + 123131
local t = aaaaaaaaaaaa.wfgoiwjofjw()
     .afjoajofjw + 123131 + 123131
aaaaaaaaaaaa.wfgoiwjofjw()
     .afjoajofjw()
)",
            R"(
return aaaaaaaaaaaa.wfgoiwjofjw()
                   .afjoajofjw + 123131 + 123131
local t = aaaaaaaaaaaa.wfgoiwjofjw()
                      .afjoajofjw + 123131 + 123131
aaaaaaaaaaaa.wfgoiwjofjw()
            .afjoajofjw()
)",
            style));
    style.align_chain_expr = AlignChainExpr::OnlyCallStmt;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
return aaaaaaaaaaaa.wfgoiwjofjw()
    .afjoajofjw + 123131 + 123131
local t = aaaaaaaaaaaa.wfgoiwjofjw()
    .afjoajofjw + 123131 + 123131
aaaaaaaaaaaa.wfgoiwjofjw()
     .afjoajofjw()
)",
            R"(
return aaaaaaaaaaaa.wfgoiwjofjw()
    .afjoajofjw + 123131 + 123131
local t = aaaaaaaaaaaa.wfgoiwjofjw()
    .afjoajofjw + 123131 + 123131
aaaaaaaaaaaa.wfgoiwjofjw()
            .afjoajofjw()
)",
            style));
}

TEST(FormatByStyleOption, align_continuous_similar_call_args) {
    LuaStyle style;

    style.align_continuous_similar_call_args = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
map("n", "<C-j>", quickfix_step("below"), { buffer = true })
map("n", "<C-k>", quickfix_step("above"), { buffer = true })
map("n", "<Space>", "<CR><C-w>p", { buffer = true })
map({ "n", "x" }, "<CR>", "<CR>", { buffer = true })
)",
            R"(
map("n", "<C-j>", quickfix_step("below"), { buffer = true })
map("n", "<C-k>", quickfix_step("above"), { buffer = true })
map("n", "<Space>", "<CR><C-w>p", { buffer = true })
map({ "n", "x" }, "<CR>", "<CR>", { buffer = true })
)",
            style));
    style.align_continuous_similar_call_args = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
map("n", "<C-j>", quickfix_step("below"), { buffer = true })
map("n", "<C-k>", quickfix_step("above"), { buffer = true })
map("n", "<Space>", "<CR><C-w>p", { buffer = true })
map({ "n", "x" }, "<CR>", "<CR>", { buffer = true })
)",
            R"(
map("n",          "<C-j>",   quickfix_step("below"), { buffer = true })
map("n",          "<C-k>",   quickfix_step("above"), { buffer = true })
map("n",          "<Space>", "<CR><C-w>p",           { buffer = true })
map({ "n", "x" }, "<CR>",    "<CR>",                 { buffer = true })
)",
            style));
}

TEST(FormatByStyleOption, break_before_braces) {
    LuaStyle style;

    style.break_before_braces = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = {
    a = 123
}
)",
            R"(
local t = {
    a = 123
}
)",
            style));
    style.break_before_braces = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = {
    a = 123
}
local t = { {
    okokok = 123
} }
)",
            R"(
local t =
{
    a = 123
}
local t =
{
    {
        okokok = 123
    }
}
)",
            style));
}

TEST(FormatByStyleOption, end_statement_with_semicolon) {
    LuaStyle style;

    style.end_statement_with_semicolon = EndStmtWithSemicolon::Keep;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local func = testFunc; print(type(nil)); -- nil;
print("Hello"); print(" "); --[[ test comment --]] print("World")
for i, v in ipairs(values) do
    if v then
        print(i, v, "True");
    else
        print(i, v, "False")
    end;
end;
local table1 = { 1, 2, 3 }
)",
            R"(
local func = testFunc; print(type(nil)); -- nil;
print("Hello"); print(" "); --[[ test comment --]] print("World")
for i, v in ipairs(values) do
    if v then
        print(i, v, "True");
    else
        print(i, v, "False")
    end;
end;
local table1 = { 1, 2, 3 }
)",
            style));

    style.end_statement_with_semicolon = EndStmtWithSemicolon::Always;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local func = testFunc; print(type(nil)); -- nil;
print("Hello"); print(" "); --[[ test comment --]] print("World")
for i, v in ipairs(values) do
    if v then
        print(i, v, "True");
    else
        print(i, v, "False")
    end;
end;
local table1 = { 1, 2, 3 }
)",
            R"(
local func = testFunc; print(type(nil)); -- nil;
print("Hello"); print(" "); --[[ test comment --]] print("World");
for i, v in ipairs(values) do
    if v then
        print(i, v, "True");
    else
        print(i, v, "False");
    end;
end;
local table1 = { 1, 2, 3 };
)",
            style));

    style.end_statement_with_semicolon = EndStmtWithSemicolon::Never;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local func = testFunc; print(type(nil)); -- nil;
print("Hello"); print(" "); --[[ test comment --]] print("World")
for i, v in ipairs(values) do
    if v then
        print(i, v, "True");
    else
        print(i, v, "False")
    end;
end;
local table1 = { 1, 2, 3 }
)",
            R"(
local func = testFunc
print(type(nil))                         -- nil;
print("Hello")
print(" ") --[[ test comment --]]
print("World")
for i, v in ipairs(values) do
    if v then
        print(i, v, "True")
    else
        print(i, v, "False")
    end
end
local table1 = { 1, 2, 3 }
)",
            style));

    style.end_statement_with_semicolon = EndStmtWithSemicolon::SameLine;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local func = testFunc; print(type(nil)); -- nil;
print("Hello"); print(" "); --[[ test comment --]] print("World")
for i, v in ipairs(values) do
    if v then
        print(i, v, "True");
    else
        print(i, v, "False")
    end;
end;
local table1 = { 1, 2, 3 }
)",
            R"(
local func = testFunc; print(type(nil))  -- nil;
print("Hello"); print(" "); --[[ test comment --]] print("World")
for i, v in ipairs(values) do
    if v then
        print(i, v, "True")
    else
        print(i, v, "False")
    end
end
local table1 = { 1, 2, 3 }
)",
            style));
}

TEST(FormatByStyleOption, keep_indents_on_empty_lines) {
    LuaStyle style;

    style.keep_indents_on_empty_lines = true;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = function()  local a = 131


    local b = function()
        local c = {
        }
    end
end
      
local t2 = function()
    local c = 1
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

local t2 = function()
    local c = 1
end

)",
            style));
    
    style.keep_indents_on_empty_lines = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = function()  local a = 131


    local b = function()
        local c = {
        }
    end
end
      
local t2 = function()
    local c = 1
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

local t2 = function()
    local c = 1
end

)",
            style));
}

TEST(FormatByStyleOption, line_space_around_block) {
    LuaStyle style;

    style.line_space_around_block = LineSpace(LineSpaceType::Keep);
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function f()

    local t = 123

end
)",
            R"(
function f()

    local t = 123

end
)",
            style));

    style.line_space_around_block = LineSpace(LineSpaceType::Max, 2);
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function f()


    local t = 123


end
)",
            R"(
function f()

    local t = 123

end
)",
            style));
}

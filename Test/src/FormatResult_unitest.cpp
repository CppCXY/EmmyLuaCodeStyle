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
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
if a then
elseif b then
end
)",
            R"(
if a then
elseif b then
end
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
if a and b then
end
)",
            R"(
if a and b then
end
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
if a == 1
and b == 2 then
end
)",
            R"(
if a == 1
    and b == 2 then
end
)"));
}

TEST(Format, LocalFunctionStatement) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local function f() return x end
)",
            R"(
local function f() return x end
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local function f()
    return x
end
)",
            R"(
local function f()
    return x
end
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local function f(a,b,c,d,e)
    return x
end
)",
            R"(
local function f(a, b, c, d, e)
    return x
end
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local function f(a,
    b,
    c,
    d,
    e
)
    return x
end
)",
            R"(
local function f(a,
                 b,
                 c,
                 d,
                 e
)
    return x
end
)"));
}

TEST(Format, WhileStatement) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
while true do break end
)",
            R"(
while true do break end
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
while true do
end
)",
            R"(
while true do
end
)"));
}

TEST(Format, DoStatement) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
do return x end
)",
            R"(
do return x end
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
do
    return x
end
)",
            R"(
do
    return x
end
)"));
}

TEST(Format, ForStatement) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
for i = 1, 2, 1 do

end
)",
            R"(
for i = 1, 2, 1 do

end
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
for i = 1, 2 do

end
)",
            R"(
for i = 1, 2 do

end
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
for i,j,k in pairs(ddd) do

end
)",
            R"(
for i, j, k in pairs(ddd) do

end
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
for i,j,k in next, t, 1 do

end
)",
            R"(
for i, j, k in next, t, 1 do

end
)"));
}

TEST(Format, RepeatStatement) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
repeat
until false
)",
            R"(
repeat
until false
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
repeat
until f1()
    and f2()
)",
            R"(
repeat
until f1()
    and f2()
)"));
}

TEST(Format, FunctionStatement) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function f() return x end
)",
            R"(
function f() return x end
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function f()
    return x
end
)",
            R"(
function f()
    return x
end
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function f(a,b,c,d)
    return x
end
)",
            R"(
function f(a, b, c, d)
    return x
end
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function t:GetInstance()
    return x
end
)",
            R"(
function t:GetInstance()
    return x
end
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function t.a.b.c:GetInstance()
    return x
end
)",
            R"(
function t.a.b.c:GetInstance()
    return x
end
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function t.a.b.c.GetInstance()
    return x
end
)",
            R"(
function t.a.b.c.GetInstance()
    return x
end
)"));
}

TEST(Format, LabelStatement) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
while true do
    if true then
        goto continue
    end
    ::continue::
end
)",
            R"(
while true do
    if true then
        goto continue
    end
    ::continue::
end
)"));
}

TEST(Format, BreakStatement) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
while true do
    if true then
        break
    end
end
)",
            R"(
while true do
    if true then
        break
    end
end
)"));
}

TEST(Format, ReturnStatement) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
return x
)",
            R"(
return x
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
return x, y, z
)",
            R"(
return x, y, z
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function wrapper(f)
    return function()
        f()
    end
end
)",
            R"(
function wrapper(f)
    return function()
        f()
    end
end
)"));
}

TEST(Format, ExpressionStatement) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
p()
)",
            R"(
p()
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
a.b.c.d()
)",
            R"(
a.b.c.d()
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
a.b.c:d(a,b,c)
)",
            R"(
a.b.c:d(a, b, c)
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
a.b.c:d(a,b,c)
)",
            R"(
a.b.c:d(a, b, c)
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
a
    .b.c:d(a,b,c)
)",
            R"(
a
    .b.c:d(a, b, c)
)"));
}

TEST(Format, Expression) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
t = a + 1
t = a.."12313"
t = -a
t = - - -a
t = (a + b) + 1
t = "hello world"
t = [[yes]] .. ddd
t = {a, b}
t = function()
end
p(function()
end)
t = p() + n.p()
)",
            R"(
t = a + 1
t = a .. "12313"
t = -a
t = - - -a
t = (a + b) + 1
t = "hello world"
t = [[yes]] .. ddd
t = { a, b }
t = function()
end
p(function()
end)
t = p() + n.p()
)"));
}

TEST(Format, Comment) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = 123--hello
)",
            R"(
local t = 123 --hello
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
p(a --[[aaa]])
)",
            R"(
p(a --[[aaa]])
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = {
    --123456
    a = 123 --789
    --10
}
)",
            R"(
local t = {
    --123456
    a = 123 --789
    --10
}
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(#! lua 1.lua
)",
            R"(#! lua 1.lua
)"));
}

TEST(Format, MultiLine) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function g(x)
    if not x then return 3 end return (x('a', 'x'))
end
)",
            R"(
function g(x)
    if not x then return 3 end
    return (x('a', 'x'))
end
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
if a then
else end
)",
            R"(
if a then
else
end
)"));
}

TEST(Format, sumenko_1915) {
    LuaStyle s;
    s.indent_size = 2;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
return nil, {
  sql = true,
  fields = true,
  field_names = true,
  extends = true,
  mixins = true,
  __index = true,
  admin = true
}
)",
            R"(
return nil, {
  sql = true,
  fields = true,
  field_names = true,
  extends = true,
  mixins = true,
  __index = true,
  admin = true
}
)", s));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = nil, {
    sql = true
}
)",
            R"(
local t = nil, {
    sql = true
}
)"));
}

TEST(Format, sumenko_1921) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
p(-1)
t[-1] = -1
)",
            R"(
p(-1)
t[-1] = -1
)"));
}

TEST(Format, codestyle_89) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = {
    a = cond
    and "is true"
    or "is not true"
}


local s = ("%s")
    :format("text")


print(("%s")
    :format("text"))


print("s1"
    .. "s2")


local f1 = function()
    print("test")
end


local f2 = cond or function()
    print("test")
end
)",
            R"(
local t = {
    a = cond
        and "is true"
        or "is not true"
}


local s = ("%s")
    :format("text")


print(("%s")
    :format("text"))


print("s1"
    .. "s2")


local f1 = function()
    print("test")
end


local f2 = cond or function()
    print("test")
end
)"));
}

TEST(Format, codestyle_90) {
    LuaStyle style;
    style.call_arg_parentheses = CallArgParentheses::Remove;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
null_ls.setup {
    sources = {
        null_ls.builtins.formatting.prettierd.with {
            disabled_filetypes = { 'html' },
        },
        null_ls.builtins.formatting.autopep8.with {
            extra_args = {
                '--indent-size=2'
            }
        },
        null_ls.builtins.formatting.xmlformat.with {

        },
    }
}
)",
            R"(
null_ls.setup {
    sources = {
        null_ls.builtins.formatting.prettierd.with {
            disabled_filetypes = { 'html' },
        },
        null_ls.builtins.formatting.autopep8.with {
            extra_args = {
                '--indent-size=2'
            }
        },
        null_ls.builtins.formatting.xmlformat.with {

        },
    }
}
)", style));
}

TEST(Format, sumneko_1969) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
Garbage = {
	    ["BahamaFrontDesk"] = {
		Coords = vector3( -1382.54, -592.77, 29.91),
		--Prop = "",
	}
}
)",
            R"(
Garbage = {
    ["BahamaFrontDesk"] = {
        Coords = vector3(-1382.54, -592.77, 29.91),
        --Prop = "",
    }
}
)"));
}

TEST(Format, codestyle_93) {
    LuaStyle s;
    s.indent_size = 2;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local context = {
  form_field_token = utils.trunk(form_field_token, 2):map(function(e)
    local form_item_token, widget = e[1], e[2]
    return format(form_item_token, widget)
  end):join('\n'),
}
local a = aaa.p(function()
end):okok()
local a2 = aaa.p(function()
    end)
    :okok()
)",
            R"(
local context = {
  form_field_token = utils.trunk(form_field_token, 2):map(function(e)
    local form_item_token, widget = e[1], e[2]
    return format(form_item_token, widget)
  end):join('\n'),
}
local a = aaa.p(function()
end):okok()
local a2 = aaa.p(function()
    end)
    :okok()
)", s));
}

TEST(Format, codestyle_98) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local function t(entry, item)
    item.dup = ({
        aa = 0
    })[entry.name] or 0

    item.dup2 = ({
        aa = 0,
    })[entry.name]

    return item
end
)",
            R"(
local function t(entry, item)
    item.dup = ({
        aa = 0
    })[entry.name] or 0

    item.dup2 = ({
        aa = 0,
    })[entry.name]

    return item
end
)"));
}

TEST(Format, feature_format_disable) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function f()
---@format disable
local t=123


okoko=-123
end
local t =123
)",
            R"(
function f()
    ---@format disable
local t=123


okoko=-123
end

local t = 123
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
function f()
---@format disable-next
local t=123


okoko=-123
end
local t =123
)",
            R"(
function f()
    ---@format disable-next
local t=123


    okoko = -123
end

local t = 123
)"));
}

TEST(Format, feature_102) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t; Init(v)
)",
            R"(
local t; Init(v)
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t; Init(v)
aaa = 123; pppp(aaa);

ddd = 123;

pcall(f, 1, 2, 3); do return true end
)",
            R"(
local t; Init(v)
aaa = 123; pppp(aaa);

ddd = 123;

pcall(f, 1, 2, 3); do return true end
)"));
}

TEST(Format, feature_106) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local empty = function () end
)",
            R"(
local empty = function() end
)"));
}

TEST(Format, issue_109) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
--[[]]


local t = 13 + 123
)",
            R"(
--[[]]


local t = 13 + 123
)"));
}

TEST(Format, feature_111) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local table = { -- My comment
    elem1 = {},
    elem2 = {}
}
)",
            R"(
local table = { -- My comment
    elem1 = {},
    elem2 = {}
}
)"));
}

TEST(Format, feature_107_ignore_unicode_align) {
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = 123
local dddd  = 456


local 你说的对 = 123
local 但是  = 456
)",
            R"(
local t    = 123
local dddd = 456


local 你说的对 = 123
local 但是 = 456
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = {
    t = 123
    dddd  = 456
}


local t = {
    你说的对 = 123
    但是  = 456
}
)",
            R"(
local t = {
    t    = 123
    dddd = 456
}


local t = {
    你说的对 = 123
    但是 = 456
}
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = { -- hello
    t = 123
    dddd = 456 -- hello
}


local t = { --hello
    你说的对 = 123 --hello
    但是 = 456 --hello
}
)",
            R"(
local t = {    -- hello
    t = 123
    dddd = 456 -- hello
}


local t = { --hello
    你说的对 = 123 --hello
    但是 = 456 --hello
}
)"));
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local t = {
    { 1, 222, 333 }
    { 4, 55, 6 }
}


local t = {
    { "你说的对", 222, 333 },
    { "但是", 55, 6 }
}
)",
            R"(
local t = {
    { 1, 222, 333 }
    { 4, 55,  6 }
}


local t = {
    { "你说的对", 222, 333 },
    { "但是", 55, 6 }
}
)"));
}

TEST(Format, bug_131) {
    LuaStyle style;

    style.space_before_inline_comment = 2;
    style.align_continuous_inline_comment = false;
    EXPECT_TRUE(TestHelper::TestFormatted(
            R"(
local table = { -- My comment
    elem1 = {}, --12313
    elem2 = {}
}
)",
            R"(
local table = {  -- My comment
    elem1 = {},  --12313
    elem2 = {}
}
)", style));
}
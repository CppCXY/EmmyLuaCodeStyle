# 格式化行为介绍

## 基本语句

算法本身会采用比较合理的原则做基本的代码排版优化：
* 所有标识/关键词/字面表达式通常会和下一个元素保持一个空格或者保持相对行距
* 双目/单目运算符会和操作数保持一个空格或者保持相对行距，逗号和前一个元素没有间距和后一个元素保持一个空格
* 短语句块可以（但不是必须）保持在同一行，这个原则对所有function/do/while/repeat/if语句有效
* 算法不会自动打断单行过长的语句，算法很少主动换行，算法会尽可能保证所有元素行间距和原来一样
* 算法不会删除任何可见元素，比如分号。如果存在语法错误则不予格式化
* 算法不会对注释的内容排版，比如长注释的所有被认为是注释的部分不会增加缩进


```lua
local t=123 --local
aaa,bbbb = 1123, 2342
local f =function() end
local d = {
    -- comment
    aa = 213, --comment
};
local e = a+b+c+d
function fff(a,b,c) end
function ff2()
--[[
    long comment
]]
end
```

会被格式化为：
```lua
local t = 123 --local
aaa, bbbb = 1123, 2342
local f = function() end
local d = {
    -- comment
    aa = 213, --comment
};
local e = a + b + c + d
function fff(a, b, c) end

function ff2()
    --[[
    long comment
]]
end
```

## 连续赋值语句

对于连续的赋值或者local语句，当算法识别到代码试图对齐到等号的时候，会采取对齐到等号的方式排版，识别对齐的基本原则是: 
    
* 连续的赋值/local/注释语句，当任意赋值/local语句的等号与它左边的元素距离大于1个空格时，该连续的赋值/local语句会对齐到等号。
* 这里对连续对齐的定义是: 语句之间相距不超过2行
* 对齐会采用最小对齐

```lua
local ttt  = 123 --first
cd        = 345 --second
```

格式化后：

```lua
local ttt = 123 --first
cd        = 345 --second
```

## 表的排版

对常见的表表达式，默认情况下的排版方式是：
* 表内的field会和表的大括号间距一个空白或者保持相对行距
* 表内的不同field会保持相对位置不变
* 如果所有field在不同行，并且均是key = value的形式，则当不同的field试图对齐到等号的时候，算法会对齐到等号
* 试图对齐到等号的基本原则是连续的key = value/注释field，当其中任意一行field的等号与它左边的元素距离大于1个空格时，该连续的key = value field会对齐到等号
* 对齐会采用最小对齐

```lua
local t = {1,2,3}
local c = {
    aaa, bbb, eee
}
local d = {
    aa  =123,
    bbbb = 4353,
    eee  = 131231,
}
```

格式化后：

```lua
local t = { 1, 2, 3 }
local c = {
    aaa, bbb, eee
}
local d = {
    aa   = 123,
    bbbb = 4353,
    eee  = 131231,
}
```
## if语句对齐

lua没有switch语句，一般都是通过if/elseif模拟出一个匹配列表，对于其他有switch语句的语言，格式化会将case对齐，这里也支持这种行为：

```lua
if aa.isDDDD()
and bb == fwfwfw
or hi == 123 then
    print(1313)
elseif cc == 123
or dd == 13131 and ddd == 123 then
    local ccc = 123
end
```
会被格式化为：
```lua
if  aa.isDDDD()
and bb == fwfwfw
or  hi == 123 then
    print(1313)
elseif cc == 123
or     dd == 13131 and ddd == 123 then
    local ccc = 123
end
```

该特性不是默认行为

## 长表达式列表

如果长表达式列表和长表达式存在不止一行，则除了首行，其余行会多一个延续行缩进：

例如：

```lua
local aaa = 13131, bbbb,
    ccc, 123131, ddddd,
    eeee, fff

return aaa, bbb, dddd
    or cccc

if aaa and bbb 
    or ccc() then

end

```

## 包含语句块的语句

对于if语句，repeat语句，while语句，do语句，for语句和函数定义语句他们都有如下格式化特性：

* 当包含语句块的语句只有一行的时候，算法会保持他们在一行内
* 通常包含语句块的语句，在语句块内会多一个缩进
* 包含语句块的语句和下一个语句通常会间距至少1行

例如：

```lua
do return end
function f(x,y) return x<y  end
function fff()
local t =123
end
```

会被格式化为：

```lua
do return end

function f(x, y) return x < y end

function fff()
    local t = 123
end

```

## 函数定义参数与函数调用参数

函数定义语句的定义参数有如下原则：
* 默认情况下函数的定义参数如果换行，则换行后会对齐到第一个参数

例如：
```lua
function ffff(a, b, c,
    callback)
end
```

格式化后：

```lua
function ffff(a, b, c,
              callback)
end

```

函数调用的参数，有如下原则：
* 如果函数有多个参数，如果参数列表本身换了行，则除了首行，其余行会多一个延续行缩进
* 如果函数调用为单参数无括号的方式则函数标识和参数之间会保持一个空格

例如：
```lua
print(aaa, bbbb, eeee, ffff,
ggggg, hhhhh,
lllll)
require "code_format"
```
格式化后：

```lua
print(aaa, bbbb, eeee, ffff,
    ggggg, hhhhh,
    lllll)
require "code_format"
```

## 注释语句与内联注释

注释语句分为长注释和单行短注释和内联注释
* 注释语句格式化时，短注释不会做任何改变，保持和下一个语句相同的行间距，而长注释的大段空白文本被认为是注释一部分，所以也不会做任何改变，同样保持和下一个语句相同的行间距
* 内联注释，采用和前文保持一个空格的原则，如果下一个元素依然在同一行，也会保持一个空格

```lua
--[[
    这是个注释
]]

---@param f number
---@return number
function ffff(f)
end

local t = 123 --fffff
local c = {
    aaa = 123, --[[ffff]] dddd,
    ccc = 456
}
```

格式化后：

```lua
--[[
    这是个注释
]]

---@param f number
---@return number
function ffff(f)
end

local t = 123 --fffff
local c = {
    aaa = 123, --[[ffff]] dddd,
    ccc = 456
}
```


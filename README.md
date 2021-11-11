# EmmyLuaCodeStyle

## 项目介绍

该项目是基于C++的lua代码格式化算法库

经过长期实践，发现人们对格式化算法的预期是尽可能少的改动代码的行布局，而列布局符合基本审美就可以了。

基于这样的想法我设计并实现了lua格式化算法

## 格式化行为介绍

### 基本语句

该算法的主要特点是分析并对当前代码做出合理的优化，最常见的比如:

```lua
local t=123 --local
aaa,bbbb = 1123, 2342
```

会被格式化为：
```lua
local t = 123 --local
aaa, bbbb = 1123, 2342
```

### 连续赋值语句

对于连续的赋值或者local语句:
```lua
local ttt  = 123 --first
cd        = 345 --second
```
当算法识别到代码试图对齐到等号的时候，会采取对齐到等号的方式排版:
```lua
local ttt  = 123 --first
cd         = 345 --second
```

识别对齐的基本原则是: 

连续的赋值/local/注释语句，当首个赋值/local语句的等号与它左边的元素距离大于1个空格时，该连续的赋值/local语句会对齐到等号。

这里对连续的定义是: 语句之间相距不超过2行则认为是同一连续

### 表的排版

对常见的表表达式:
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
默认情况下算法会排版为如下格式:
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

其中对 d 表的排版并非严格对齐到等号，它采取和连续赋值/local语句相似的规则，但是当表内存在处于同一行得表项时，算法不会对齐到等号。

### 长表达式列表

长表达式列表和长表达式如果不止一行，则除了第一行，其余行会多一个缩进。
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

### 包含语句块的语句

对于if语句，repeat语句，while语句，do语句，for语句和函数定义语句，都具备保持短语句不会强行换行的特性也就是说，如下语句：
```lua
do return end
function f(x,y) return x<y  end
```
会被格式化为：
```lua
do return end
function f(x, y) return x < y end
```
除此以外，包含语句块的语句，在语句块内会多一个缩进语句。

同时经过长期的实践，包含语句块的语句应与下一个非注释语句保持至少一行的距离：
```lua
do 
    if a then
        return 
    end

    local c = 123
end

print("hello world")
```

### 函数定义参数与函数调用参数

对于函数定义语句的定义参数，默认采用对齐到首个参数的方式:
```lua
function ffff(a, b, c,
              callback)

end
```
对于函数调用的参数，默认采用换行多一个缩进的策略,
```lua
print(aaa, bbbb, eeee, ffff,
    ggggg, hhhhh,
    lllll)
```
如果函数调用为单参数无括号的方式则函数标识和参数之间会保持一个空格
```lua
require "code_format"
```
### 注释语句与内联注释

注释语句分为长注释和单行短注释，内联注释包括插入到所有语法元素之后/中间的注释。

注释语句格式化时，短注释不会做任何改变，保持和下一个语句相同的行间距，而长注释的大段空白文本被认为是注释一部分，所以也不会做任何改变，同样保持和下一个语句相同的行间距：
```lua
--[[
    这是个注释
]]

---@param f number
---@return number
function ffff(f)
end
```
对于各种内联注释，采用和前文保持一个空格的原则，如果下一个元素依然在同一行，也会保持一个空格:
```lua
local t = 123 --fffff
local c = {
    aaa = 123, --[[ffff]] dddd,
    ccc = 456
}

```

以上只是默认选项下的格式化结果，该算法依然提供各种细节到每个语法元素的排版控制

## 格式化选项

通常项目内为了保持代码风格统一需要十分详尽的格式化配置，另一方面默认的格式化选项，可能不符合每个人的审美。这里逐步介绍所有的格式化选项和效果。

该项目支持从editorconfig文件读取配置来配置格式化选项，完整的配置参考本项目自带的template.editorconfig。

### use_tab

该选项表示是否使用\t来做缩进，该选项默认为false，如果该选项为true，则选项indent无效，格式化效果如下：
```lua
--格式化前
function ff() 
    local t = 123
    local c = 456
end

--格式化后
function ff()
        local t = 123
        local c = 456
end
```

### indent

该选项表示单次缩进的列数，默认值为4，各种常见的选项下的效果如下：
```lua
--格式化前
function ff() 
    local t = 123
    local c = 456
end

-- indent = 2
function ff()
  local t = 123
  local c = 456
end

-- indent = 3
function ff()
   local t = 123
   local c = 456
end

-- indent = 4
function ff()
    local t = 123
    local c = 456
end
```

### continuation_indent

该选项表示长表达式或者表达式列表在换行表达时的后续缩进列数，该选项默认值是4，常见的还有8，格式化效果如下：
```lua
-- 格式化前
local fff = function(x,y)
end

local ccc = aa + bbb + ccc /
    ddd + eee

if aaa == 123 
or bbb == 456 then
end

-- 值为4时的格式化结果
local fff = function(x, y)
end
local ccc = aa + bbb + ccc /
    ddd + eee

if aaa == 123
    or bbb == 456 then
end

-- 值为8时的格式化结果
local fff = function(x, y)
end
local ccc = aa + bbb + ccc /
        ddd + eee

if aaa == 123
        or bbb == 456 then
end

```

### align_call_args

该参数表示调用参数是否对齐，默认值是false，格式化效果如下：
```lua
-- 格式化前
helloWorld(aaa,bbb,ccc,
eee,ddd)

-- 格式化后
-- align_call_args 为true
helloWorld(aaa, bbb, ccc,
           eee, ddd)

-- align_call_args 为false
helloWorld(aaa, bbb, ccc,
    eee, ddd)

```

### keep_one_space_between_call_args_and_bracket

该选项表示函数调用表达式的参数和左右括号之间是否保持一个空格，选项默认值是false，格式化效果如下
```lua
-- 格式化前
helloWorld(aaa,bbb,ccc,
eee,ddd)

print(123,456)

-- 该选项为true时
helloWorld( aaa, bbb, ccc,
    eee, ddd )

print( 123, 456 )

```

### align_function_define_params

该选项表示函数定义语句的参数列表是否对齐到第一个参数的位置，选项默认为true，格式化效果如下：
```lua
-- 格式化前
function fffff(aaa,bbb,ccc)
end

function ddddd(eeee,ffff,
    ggg,hhh)
end

-- 该选项为true时
function fffff(aaa, bbb, ccc)
end

function ddddd(eeee, ffff,
               ggg, hhh)
end

-- 该选项为false时
function fffff(aaa, bbb, ccc)
end

function ddddd(eeee, ffff,
    ggg, hhh)
end
```

### keep_one_space_between_table_and_bracket

该选项表示表的表项和表的括号之间是否保持一个空格，该选项默认为true，格式化效果如下：
```lua
-- 格式化前
local t = {1,2,3}
local cc ={
    4,5,6
}

-- 该选项为true时
local t = { 1, 2, 3 }
local cc = {
    4, 5, 6
}
-- 该选项为false时
local t = {1, 2, 3}
local cc = {
    4, 5, 6
}
```

### align_table_field_to_first_field

该选项表示表的表项是否对齐到首个表项，选项默认值为true，格式化效果如下：
```lua
-- 格式化前
local t = {
    aaa,bbbb,ccc,
    ddd,eee,fff
}

local t2 = {aaa,bbbb,
ccc,eee
}


-- 该选项为true
local t = {
    aaa, bbbb, ccc,
    ddd, eee, fff
}
local t2 = { aaa, bbbb,
             ccc, eee
}

-- 该选项为false
local t = {
    aaa, bbbb, ccc,
    ddd, eee, fff
}
local t2 = { aaa, bbbb,
    ccc, eee
}
```

### continuous_assign_statement_align_to_equal_sign

该选项表示连续赋值语句的首行语句如果等号与左边符号相距大于一个空格，则对齐到连续赋值语句的等号最大的位置，该选项默认值为true，效果如下：
```lua
-- 格式化前
local t  = 123
local cccc = 456
cccc = 321
-- 这是个注释
eeeeeeeee = 654 -- 这也是个注释


--不连续了
local c =132

-- 选项为true时
local t    = 123
local cccc = 456
cccc       = 321
-- 这是个注释
eeeeeeeee  = 654 -- 这也是个注释


--不连续了
local c = 132

-- 选项为false时
local t = 123
local cccc = 456
cccc = 321
-- 这是个注释
eeeeeeeee = 654 -- 这也是个注释


--不连续了
local c = 132
```

### continuous_assign_table_field_align_to_equal_sign

该选项表示表中各项是否允许对齐到等号，对齐的规则和上面的选项近似，选项默认值为true，格式化效果如下：
```lua
-- 格式化前
local t = {
    aadddd  = 123,
    bbb =456,
    ddd =789
}

-- 选项为true时
local t = {
    aadddd  = 123,
    bbb     = 456,
    ddd     = 789
}
-- 选项为false时
local t = {
    aadddd = 123,
    bbb = 456,
    ddd = 789
}
```

### line_separator

该选项表示行尾的符号，默认为CRLF，也可选为LF

### 语句的行布局

该算法同样支持对不同语句之间的行间距做出设定，可以设定的语句包括if, while, repeat, for，do，local/assign 语句。

这些选项名称和默认值分别是：
```
keep_line_after_if_statement = minLine:1
keep_line_after_do_statement = minLine:1
keep_line_after_while_statement = minLine:1
keep_line_after_repeat_statement = minLine:1
keep_line_after_for_statement = minLine:1
keep_line_after_local_or_assign_statement = keepLine
keep_line_after_function_define_statement = keepLine:1
```

所有选项支持三种值表达：
- minLine:${n}
    
    表示和下一个语句间距至少n行
- keepLine

    表示和下一行保持原本的行间距
- keepLine:${n}

    表示和下一行保持n行间距

## 代码风格诊断

待续

## License

没有License

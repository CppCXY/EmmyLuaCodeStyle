# 格式化选项

通常项目内为了保持代码风格统一需要仔细的配置，另一方面默认的格式化选项，可能不符合每个人的审美。

本项目提供基于editorconfig的方式配置格式化，完整的配置参考本项目自带的lua.template.editorconfig。

## indent_style

该选项指定缩进时使用的不可见字符，可选的值为 space/tab

格式化效果如下：

```lua
--use space
function ff() 
    local t = 123
    local c = 456
end

--use tab
function ff()
    local t = 123
    local c = 456
end
```

## indent_size

该选项表示单次缩进的列数，默认值为4，仅当indent_style 为 space 时有效

各种常见的选项下的效果如下：

```lua
-- default
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

## tab_width

该选项仅当indent_style为tab时有效
该选项的效果通常与编辑器有关，这里不做展示。

## continuation_indent_size

该选项表示长表达式或者表达式列表在换行表达时的后续缩进列数，该选项默认值是4，常见的还有8

格式化前

```lua
-- 
local fff = function(x,y)
end

local ccc = aa + bbb + ccc /
    ddd + eee

if aaa == 123 
or bbb == 456 then
end

```
格式化后：

```lua

-- = 4
local fff = function(x, y)
end
local ccc = aa + bbb + ccc /
    ddd + eee

if aaa == 123
    or bbb == 456 then
end

-- = 8
local fff = function(x, y)
end
local ccc = aa + bbb + ccc /
        ddd + eee

if aaa == 123
        or bbb == 456 then
end
```
## local_assign_continuation_align_to_first_expression

该参数表示local或者赋值语句的右表达式列表，如果存在不止一行则对齐到首个表达式，例如：
```lua
local t = aaa,bbb,ccc,
    ddd,eee,fff
```
会被格式化为
```lua
local t = aaa,bbb,ccc,
          ddd,eee,fff
```

当该选项有效时，continuation_indent_size选项对local和赋值语句无效。
当表达式列表中出现跨行表达式，例如跨行定义的function 和 table，则该选项无效。


## align_call_args

该参数表示调用参数是否对齐到首个参数，默认值是false

格式化前：

```lua
-- 格式化前
helloWorld(aaa,bbb,ccc,
eee,ddd)

```
格式化后：

```lua
-- align_call_args = true
helloWorld(aaa, bbb, ccc,
           eee, ddd)

-- align_call_args = false
helloWorld(aaa, bbb, ccc,
    eee, ddd)

```

当调用参数列表中出现跨行表达式时，该选项定义的行为无效

## keep_one_space_between_call_args_and_parentheses

该选项表示函数调用表达式的参数和左右括号之间是否保持一个空格，选项默认值是false

格式化前：

```lua
helloWorld(aaa,bbb,ccc,
eee,ddd)

print(123,456)

```

格式化后：

```lua
--keep_one_space_between_call_args_and_parentheses = true

helloWorld( aaa, bbb, ccc,
    eee, ddd )

print( 123, 456 )

```

## align_function_define_params

该选项表示函数定义语句的参数列表是否对齐到第一个参数的位置，选项默认为true，格式化效果如下：

格式化前:

```lua

function fffff(aaa,bbb,ccc)
end

function ddddd(eeee,ffff,
    ggg,hhh)
end
```
格式化后：
```lua

-- true
function fffff(aaa, bbb, ccc)
end

function ddddd(eeee, ffff,
               ggg, hhh)
end

-- false
function fffff(aaa, bbb, ccc)
end

function ddddd(eeee, ffff,
    ggg, hhh)
end
```

## keep_one_space_between_table_and_bracket

该选项表示表的表项和表的括号之间是否保持一个空格，该选项默认为true。

格式化前：

```lua
local t = {1,2,3}
local cc ={
    4,5,6
}
```
格式化后：

```lua
-- true
local t = { 1, 2, 3 }
local cc = {
    4, 5, 6
}
-- false
local t = {1, 2, 3}
local cc = {
    4, 5, 6
}
```

## align_table_field_to_first_field

该选项表示表的表项是否对齐到首个表项，选项默认值为true。

格式化前：

```lua

local t = {
    aaa,bbbb,ccc,
    ddd,eee,fff
}

local t2 = {aaa,bbbb,
ccc,eee
}
```

格式化后:

```lua

-- = true
local t = {
    aaa, bbbb, ccc,
    ddd, eee, fff
}
local t2 = { aaa, bbbb,
             ccc, eee
}

-- false
local t = {
    aaa, bbbb, ccc,
    ddd, eee, fff
}
local t2 = { aaa, bbbb,
    ccc, eee
}
```

## keep_one_space_between_namedef_and_attribute

该选项表示local语句的名称列表中的名称定义和他的attribute之间保持一个空格。

格式化前：
```lua
local t<const> = 1
```

格式化后：

```lua
local t <const> = 1
```

## max_continuous_line_distance

该选项表示连续行的定义，它的值决定行之间的间距小于等于多少时为同一连续。


## continuous_assign_statement_align_to_equal_sign

该选项表示连续赋值语句的任意一行如果等号与左边符号相距大于一个空格，则采用最小对齐原则对齐到等号，该选项默认值为true.

格式化前:

```lua

local t  = 123
local cccc = 456
cccc = 321
-- this is a comment
eeeeeeeee = 654 -- this is a comment2


-- no continuous
local c =132
```

格式化后：

```lua
-- true
local t    = 123
local cccc = 456
cccc       = 321
-- this is a comment
eeeeeeeee  = 654 -- this is a comment2


-- no continuous
local c = 132
```

```lua
-- false
local t = 123
local cccc = 456
cccc = 321
-- this is a comment
eeeeeeeee = 654 -- this is a comment2


-- no continuous
local c = 132
```

## continuous_assign_table_field_align_to_equal_sign

该选项表示表中各项是否允许对齐到等号，对齐的规则和上面的选项近似，选项默认值为true

格式化前:

```lua
local t = {
    aadddd  = 123,
    bbb =456,
    ddd =789
}
```

格式化：

```lua
-- true
local t = {
    aadddd  = 123,
    bbb     = 456,
    ddd     = 789
}
-- false
local t = {
    aadddd = 123,
    bbb = 456,
    ddd = 789
}
```

## label_no_indent

该选项表示标签无缩进。

格式化前：
```lua
function fff()

    for i=1,2,3 do
        if true then
            goto continue
        end

        ::continue::
    end

end
```
格式化后:
```lua
function fff()

    for i=1,2,3 do
        if true then
            goto continue
        end

    ::continue::
    end

end
```
## do_statement_no_indent

该选项表示do语句块内无缩进。


格式化前：
```lua
function fff()
    do
        for i=1,2,3 do
            if true then
                goto continue
            end

            ::continue::
        end
    end
end
```
格式化后:
```lua
function fff()
    do
    for i=1,2,3 do
        if true then
            goto continue
        end

        ::continue::
    end
    end
end
```

## if_condition_no_continuation_indent

该选项表示if语句的条件表达式如果跨行，则无需缩进。

格式化前：
```lua
if a
    and b then

end
```
格式化后：
```lua
if a
and b then

end
```

## table_append_expression_no_space

该选项表示形如t[#t+1]的表达式，不会被格式化为t[#t + 1]

## if_condition_align_with_each_other 

该选项表示if语句的条件表达式会互相对齐，该选项当且仅当存在 elseif语句时有效，并且该选项有效时，if语句的条件表达式一定会无缩进。
互相对齐指的是像switch case一样排版:

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

## end_of_line

该选项表示行尾的符号，默认为crlf，也可选为lf

# 语句的行布局

该算法同样支持对不同语句之间的行间距做出设定，可以设定的语句包括if, while, repeat, for，do，local/assign 语句。

这些选项名称和默认值分别是：
```
keep_line_after_if_statement = minLine:0
keep_line_after_do_statement = minLine:0
keep_line_after_while_statement = minLine:0
keep_line_after_repeat_statement = minLine:0
keep_line_after_for_statement = minLine:0
keep_line_after_local_or_assign_statement = keepLine
keep_line_after_function_define_statement = keepLine:0
```

所有选项支持三种值表达：
- minLine:${n}
    
    表示和下一个语句间距至少n行
- keepLine

    表示和下一行保持原本的行间距
- keepLine:${n}

    表示和下一行保持n行间距

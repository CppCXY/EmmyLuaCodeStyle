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

## keep_one_space_between_call_args_and_bracket

该选项表示函数调用表达式的参数和左右括号之间是否保持一个空格，选项默认值是false

格式化前：

```lua
helloWorld(aaa,bbb,ccc,
eee,ddd)

print(123,456)

```

格式化后：

```lua
--keep_one_space_between_call_args_and_bracket = true

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

## continuous_assign_statement_align_to_equal_sign

该选项表示连续赋值语句的首行语句如果等号与左边符号相距大于一个空格，则对齐到连续赋值语句的等号最大的位置，该选项默认值为true.

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

## end_of_line

该选项表示行尾的符号，默认为crlf，也可选为lf

# 语句的行布局

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

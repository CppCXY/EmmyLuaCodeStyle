# 格式化选项

通常项目内为了保持代码风格统一需要仔细的配置，另一方面默认的格式化选项，可能不符合每个人的审美。

本项目提供基于editorconfig的方式配置格式化，完整的配置参考本项目自带的[lua.template.editorconfig](../lua.template.editorconfig)。


## 基本选项

### indent_style

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

### indent_size

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

### tab_width

该选项仅当indent_style为tab时有效
该选项的效果通常与编辑器有关，这里不做展示。

### quote_style

该选项表示所有使用引号表达的字符串字面值是否统一到单引号或者双一样或者保持原状，该选项有三个可选值 none/single/double

当quote_style = single 或者double时，如果字符串内存在未转义的目标引号类型，则不予格式化。

### call_arg_parentheses

该选项表示，调用表达式中如果参数仅有单字符串字面值常量或者表表达式，则可以考虑保持或者移除括号。
该选项可选值是 keep/remove/remove_table_only/remove_string_only 默认值是keep。

```lua
f("wfwefw")({ e1231313 })("1e1231313131")

```

格式化后:

```lua
--keep
f("wfwefw")({ e1231313 })("1e1231313131")
--remove
f "wfwefw" { e1231313 } "1e1231313131"
--remove_table_only
f("wfwefw") { e1231313 } ("1e1231313131")
--remove_string_only
f "wfwefw" ({ e1231313 }) "1e1231313131"
```

### continuation_indent

该选项表示长表达式或者表达式列表在换行表达时的后续缩进列数，该选项默认值是4，常见的还有8

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

### max_line_length

该选项表示每一行的最大行宽，如果一些可打断的结构超过该行宽则会打断到下一行, 默认值是120

### end_of_line

该选项指示每一行的行尾使用crlf/cr/lf中的哪种，也可以为auto，他表示在windows上使用crlf，在其他平台使用lf。默认值是 auto

注意：neovim内建的支持了editorconfig作为基本格式配置，auto并不是editorconfig支持的值，如果不希望neovim警告，该值可以设置为unset，或者不明写该选项。

### trailing_table_separator

该选项表示如何处理表的末尾项的分隔符，有四个选项keep/never/always/smart

* keep 表示原来是怎么样就怎么样
* never 表示一律移除末尾项的分隔符
* always 表示一律添加末尾项的分隔符
* smart 表示如果表的所有项如果在同一行则移除末尾项的分隔符，否则添加末尾分隔符

### detect_end_of_line

该选项表示是否由工具自行检测行尾，如果本身是crlf/cr/lf则分别格式化为原本的行尾，如果是混合行尾则参考end_of_line

### insert_final_newline

该选项表示是否以新行结尾

## 空白选项

### space_around_table_field_list

该选项表示是否为有内容的表在field列表和表的两个大括号之间添加空格，默认值为true

```lua
local t = {1,2,3}
```
格式化后：

```lua
-- true
local t = { 1, 2, 3 }
-- false
local t = {1, 2, 3}
```

### space_before_attribute

该选项表示是否为lua5.4的attribute语法添加空白, 默认值是true

```lua
local t<const> = 1
```

格式化后：

```lua
--true
local t <const> = 1
--false
local t<const> = 1
```

### space_before_function_open_parenthesis

该选项表示是否在函数的定义中的小括号之前添加一个空白，默认值是false

### space_before_function_call_open_parenthesis

该选项表示是否在函数的调用中的小括号之前添加一个空白，默认值是false

### space_before_closure_open_parenthesis

该选项表示是否在闭包表达式的定义中的小括号之前添加一个空白，默认值是false

### space_before_function_call_single_arg

该选项表示是否在函数的单参数调用形式的参数之前添加一个空白，默认值是true

### space_before_open_square_bracket

该选项表示是否在索引表达式中的中括号之前添加一个空白，默认值是false

### space_inside_function_call_parentheses

该选项表示是否在函数调用表达式的参数列表和括号之间保持一个空白

### space_inside_function_param_list_parentheses

该选项表示是否在函数定义语句的参数列表和括号之间保持一个空白

### space_inside_square_brackets

该选项表示是否在中括号和索引之间添加一个空白

### space_around_table_append_operator

该选项为lua中的一种特殊用法提供特殊的支持，形如`t[#t+1]`的表达式被识别为append方法，
该表达形式的'+'号周围是否保持空格，true表示不保留空格，false表示和正常的设定一样

```lua
t[#t+1] = 1
```

格式化后
```lua
--true
t[#t+1] = 1
--false
t[#t + 1] = 1
```

### ignore_spaces_inside_function_call

该选项表示是否忽略函数调用语句的参数列表中的空白的格式化

### space_before_inline_comment

该选项表示，如果注释是内联注释则它与前一个token保持一定的空白，默认值是1

## 操作符空白

### space_around_math_operator

该选项表示数学运算符周围是否保持空白，默认为true

### space_after_comma

该选项表示逗号之后是否需要一个空白，默认为true

### space_after_comma_in_for_statement

该选项表示在for语句中的列表中的逗号之后是否需要一个空白，默认为true

### space_around_concat_operator

该选项表示lua的字符串连接符号周围是否保留一个空白，默认为true

## 对齐

### align_call_args

该参数表示调用参数是否对齐到首个参数，默认值是false

```lua
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

### align_function_params

该选项表示函数定义语句的参数列表是否对齐到第一个参数的位置，选项默认为true

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

### align_continuous_assign_statement

该选项表示连续赋值语句的任意一行如果等号与左边符号相距大于一个空格，则采用最小对齐原则对齐到等号，该选项默认值为true

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

### align_continuous_rect_table_field

该选项表示表中各项是否允许对齐到等号，对齐的规则和上面的选项近似，选项默认值为true

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

### align_if_branch

该选项表示是否对齐if语句的各分支，包含由 `and` 和 `or` 构成的分支, 默认为false

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

格式化后：
```lua
if     aa.isDDDD()
and    bb == fwfwfw
or     hi == 123 then
    print(1313)
elseif cc == 123
or     dd == 13131 and ddd == 123 then
    local ccc = 123
end
```

### align_array_table

该选项表示是否对齐表中连续的数组项，默认为true

格式化的效果如下：
```lua
local t = {
    { "fwfwf", njqoifjiowiof, 121313 },
    { "fwfw",  fjwofw,        wngjwoigw },
    { a,       b,             c,        d },
    { 1,       2,             3 },
}
```

## 其他缩进设置

### never_indent_before_if_condition

该选项表示if语句的条件表达式如果跨行，则无需缩进。

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
### never_indent_comment_on_if_branch

该选项表示if/elseif/else之上的注释不再缩进

## 语句的行布局

该工具支持对不同语句之间的行间距做出设定，可以设定的语句包括if, while, repeat, for，do，local/assign，comment语句。

这些选项名称和默认值分别是：
```
line_space_after_if_statement = keep

line_space_after_do_statement = keep

line_space_after_while_statement = keep

line_space_after_repeat_statement = keep

line_space_after_for_statement = keep

line_space_after_local_or_assign_statement = keep

line_space_after_function_statement = fixed(2)

line_space_after_expression_statement = keep

line_space_after_comment = keep
```

所有选项支持三种值表达：
- min(${n})
    
    表示和下一个语句间距至少n行
- keep

    表示和下一行保持原本的行间距
- fixed(${n})

    表示和下一行固定保持n行间距
- max(${n})
	
	表示和下一行最多n行间距

## 行打断

### break_all_list_when_line_exceed

该选项表示当具有列表的表达式超过行宽设定时，实行全部打断，也就是打断到每一行，默认值为false，当前仅对表有效

### auto_collapse_lines

该选项表示在列表上表达式在计算后不应该被打断，则会全部塌缩到同一行，默认值为false，当前仅对表有效

## 偏好

### ignore_space_after_colon

该选项表示忽略冒号之后的空白的格式化

### remove_call_expression_list_finish_comma

该选项表示移除调用表达式中末尾逗号（这是一个扩展语法）


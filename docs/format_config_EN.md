# Formatting options

Usually, in order to maintain a uniform code style, careful configuration is required in the project. On the other hand, the default formatting options may not meet everyone's aesthetics.

This project provides configuration and formatting based on editorconfig. For complete configuration, please refer to the lua.template.editorconfig that comes with this project.

## indent_style

This option specifies the invisible characters used when indenting, the optional value is space/tab

The formatting effect is as follows: 

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

This option indicates the number of columns for a single indentation, the default value is 4, only valid when indent_style is space

The effects of various common options are as follows: 

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

This option is only valid when indent_style is tab
The effect of this option is usually related to the editor and will not be shown here.

## quote_style

This option indicates whether all string literals expressed in quotation marks are unified into single quotation marks or double or remain the same. This option has three optional values none/single/double

When quote_style = single or double, if there is an unescaped target quote type in the string, it will not be formatted. 

## call_arg_parentheses

This option means that if the parameter in the call expression has only a single-string literal constant or a table expression, consider keeping or removing the parentheses.
The optional value of this option is keep/remove/remove_table_only/remove_string_only The default value is keep.

```lua
f("wfwefw")({ e1231313 })("1e1231313131")
```

after formatting

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

If the value is unambiguous_remove_string_only, the parentheses of the string in the call expression will be removed when there is no ambiguity, for example:
```lua
local t = require("aaaa").bbb
local d = require("cccc")
```
will be formatted as:
```lua
local t = require("aaaa").bbb
local d = require "cccc"
```


## continuation_indent_size

This option indicates the number of subsequent indented columns when a long expression or expression list is expressed in a new line. The default value of this option is 4, and 8 is common

Before formatting:

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

After formatting: 

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

This parameter represents a list of local or right expressions of an assignment statement. If there is more than one line, it will be aligned to the first expression, for example:

```lua
local t = aaa,bbb,ccc,
     ddd,eee,fff
```
will be formatted as

```lua
local t = aaa,bbb,ccc,
           ddd,eee,fff
```

When this option is in effect, the continuation_indent_size option has no effect on local and assignment statements.
When a cross-line expression appears in the expression list, such as function and table defined across lines, this option is invalid. 

## align_call_args

This parameter indicates whether the calling parameter is aligned to the first parameter, the default value is false

Before formatting: 

```lua
helloWorld(aaa,bbb,ccc,
eee,ddd)

```
after formatting:

```lua
-- align_call_args = true
helloWorld(aaa, bbb, ccc,
           eee, ddd)

-- align_call_args = false
helloWorld(aaa, bbb, ccc,
    eee, ddd)

```

The behavior defined by this option has no effect when a cross-line expression appears in the call parameter list 

## align_function_define_params

This option indicates whether the parameter list of the function definition statement is aligned to the position of the first parameter, the option is true by default, and the formatting effect is as follows:

Before formatting: 

```lua

function fffff(aaa,bbb,ccc)
end

function ddddd(eeee,ffff,
    ggg,hhh)
end
```

after formatting：

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

This option indicates whether there is a space between the table entry and the table brackets. This option is true by default.

Before formatting: 

```lua
local t = {1,2,3}
local cc ={
    4,5,6
}
```

after formatting：

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

This option indicates whether the table entry is aligned to the first entry. The default value of the option is true.

Before formatting: 

```lua

local t = {
    aaa,bbbb,ccc,
    ddd,eee,fff
}

local t2 = {aaa,bbbb,
ccc,eee
}
```

after formatting:

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

This option indicates that a space should be maintained between the name definition in the name list of the local statement and its attribute.

Before formatting:
```lua
local t<const> = 1
```

After formatting:

```lua
local t <const> = 1
```

## max_continuous_line_distance

This option indicates the definition of continuous lines, and its value determines the same continuous when the spacing between lines is less than or equal to how much. 

## continuous_assign_statement_align_to_equal_sign

This option means that any line of a continuous assignment statement that is aligned to the equal sign using the principle of least alignment if the equal sign is greater than one space apart from the left symbol, and the default value for this option is true.

Before formatting: 

```lua

local t  = 123
local cccc = 456
cccc = 321
-- this is a comment
eeeeeeeee = 654 -- this is a comment2


-- no continuous
local c =132
```

after formatting：

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

This option indicates whether the items in the table are allowed to be aligned to the equal sign. The alignment rules are similar to the above options. The default value of the option is true

Before formatting: 

```lua
local t = {
    aadddd  = 123,
    bbb =456,
    ddd =789
}
```

after formatting：

```lua
-- true
local t = {
    aadddd = 123,
    bbb    = 456,
    ddd    = 789
}
-- false
local t = {
    aadddd = 123,
    bbb = 456,
    ddd = 789
}
```

## label_no_indent

This option means that the labels are not indented.

Before formatting:
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
After formatting:
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

This option means no indentation inside the do statement block.


Before formatting:
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
After formatting:
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

This option indicates that the conditional expression of the if statement does not need to be indented if it spans lines.

Before formatting:
```lua
if a
    and b then

end
```
After formatting:
```lua
if a
and b then

end
```

## table_append_expression_no_space

This option represents an expression of the form t[#t+1], which will not be formatted as t[#t + 1]

## if_condition_align_with_each_other

This option means that the conditional expressions of the if statement will be aligned with each other. This option is valid if and only if there is an elseif statement, and when this option is valid, the conditional expressions of the if statement must be without indentation.
Alignment with each other refers to typesetting like a switch case:

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
after formatting：
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

This option represents the symbol at the end of the line, the default is crlf, and it can also be lf 

# Statement line layout 

The algorithm also supports setting the line spacing between different sentences. The sentences that can be set include if, while, repeat, for, do, and local/assign sentences.

The names and default values of these options are: 
```
keep_line_after_if_statement = minLine:1
keep_line_after_do_statement = minLine:1
keep_line_after_while_statement = minLine:1
keep_line_after_repeat_statement = minLine:1
keep_line_after_for_statement = minLine:1
keep_line_after_local_or_assign_statement = keepLine
keep_line_after_function_define_statement = keepLine:1
keep_line_after_expression_statement = keepLine
```

All options support three value expressions:
- minLine:${n}
    
    Indicates that there is at least n lines between the next sentence
- keepLine

    Means to maintain the original line spacing with the next line
- keepLine:${n}

    Means to maintain n line spacing with the next line 
- maxLine:${n}

	Indicates and the next line at most n lines

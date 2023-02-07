# Formatting options

Usually careful configuration is required to keep the code style uniform within the project, on the other hand, the default formatting options may not be in line with everyone's aesthetics.

This project provides configuration formatting based on editorconfig, and the complete configuration refers to the [lua.template.editorconfig](../lua.template.editorconfig)。


## Basic options

### indent_style

This option specifies the invisible characters to use when indenting, and the optional value is space/tab

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

### indent_size

This option represents the number of columns for a single indentation, with a default value of 4 and is valid only when indent_style is space

The effect under various common options is as follows:

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

This option is valid only when the indent_style is tab
The effect of this option is usually editor-related and is not shown here.

### quote_style

This option indicates whether all string literals expressed in quotation marks are unified into single quotes or double or left as is, and this option has three optional values: none/single/double

When quote_style = single or double, unescaped target quote types are not formatted if the string exists.

### call_arg_parentheses

This option means that if the parameter in the call expression has only a single-string literal constant or a table expression, you can consider keeping or removing the parentheses.
The optional value for this option is keep/remove/remove_table_only/remove_string_only and the default value is keep.

```lua
f("wfwefw")({ e1231313 })("1e1231313131")

```

After formatting:

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

This option indicates the number of subsequent indented columns for long expressions or expressions when expressed in line wraps, and the default value of this option is 4, and 8 is common

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

### max_line_length

This option indicates the maximum line width of each row, and if some breakable structure exceeds the line width, it will break to the next line, the default value is 120

### end_of_line

This option indicates which of CRF/CR/LF is used at the end of each line, or auto, which means that CRLF is used on Windows and LF is used on other platforms. The default value is auto

Note: neovim has built-in support for editorconfig as a basic format configuration, auto is not a value supported by editorconfig, if you do not want neovim warning, the value can be set to unset, or the option is not specified.

### trailing_table_separator

This option represents how to handle the separator of the entry at the end of the table, and there are four options: keep/never/always/smart

* Keep means how it was
* never indicates that the separator for the last item is always removed
* always indicates that the separator for the last item is always added
* smart means that if all items in the table are in the same row, the separator for the last item is removed, otherwise the end separator is added

### detect_end_of_line

This option indicates whether the tool will detect the endings by itself, and format them as the original endings if they are crlf/cr/lf, and refer to the end_of_line if they are mixed

### insert_final_newline

This option indicates whether to end with a new line

## Whitespace option

### space_around_table_field_list

This option indicates whether to add a space between the field list and the two curly braces of the table for tables with contents, and the default value is true

```lua
local t = {1,2,3}
```
After formatting:

```lua
-- true
local t = { 1, 2, 3 }
-- false
local t = {1, 2, 3}
```

### space_before_attribute

This option indicates whether to add blanks to the attribute syntax of lua 5.4, the default value is true

```lua
local t<const> = 1
```

After formatting:

```lua
--true
local t <const> = 1
--false
local t<const> = 1
```

### space_before_function_open_parenthesis

This option indicates whether to add a blank space before parentheses in the definition of the function, and the default value is false

### space_before_function_call_open_parenthesis

This option indicates whether to add a blank space before parentheses in the function call, and the default value is false

### space_before_closure_open_parenthesis

This option indicates whether to add a blank space before the parentheses in the definition of the closure expression, and the default value is false

### space_before_function_call_single_arg

This option indicates whether to add a blank space before the parameter in the form of a single-parameter call to the function, and the default value is true

### space_before_open_square_bracket

This option indicates whether to add a blank space before the brackets in the index expression, and the default value is false

### space_inside_function_call_parentheses

This option indicates whether to maintain a blank space between the argument list and parentheses of the function call expression

### space_inside_function_param_list_parentheses

This option indicates whether to maintain a blank space between the parameter list and parentheses of the function definition statement

### space_inside_square_brackets

This option indicates whether to add a blank space between brackets and index

### space_around_table_append_operator

This option provides special support for a particular usage in Lua, where expressions of the form 't[#t+1]' are recognized as the APP method.
Whether spaces are maintained around the '+' sign in this expression, true means no spaces are preserved, false means that the normal setting is the same

```lua
t[#t+1] = 1
```

After formatting
```lua
--true
t[#t+1] = 1
--false
t[#t + 1] = 1
```

### ignore_spaces_inside_function_call

This option indicates whether to ignore the formatting of blanks in the argument list of the function call statement

### space_before_inline_comment

This option means that if the comment is an inline comment, it remains somewhat blank with the previous token, and the default value is 1

## Operator blank

### space_around_math_operator

This option indicates whether the mathematical operator remains blank around and is true by default

### space_after_comma

This option indicates whether a blank space is required after the comma and defaults to true

### space_after_comma_in_for_statement

This option indicates whether a blank space is required after the comma in the list in the for statement, which defaults to true

### space_around_concat_operator

This option indicates whether there is a blank space around the string connection symbol of the lua, which is true by default

## Alignment

### align_call_args

This parameter indicates whether the call parameter is aligned to the first parameter, and the default value is false

```lua
helloWorld(aaa,bbb,ccc,
eee,ddd)

```
After formatting:

```lua
-- align_call_args = true
helloWorld(aaa, bbb, ccc,
           eee, ddd)

-- align_call_args = false
helloWorld(aaa, bbb, ccc,
    eee, ddd)

```

### align_function_params

This option indicates whether the parameter list of the function definition statement is aligned to the position of the first parameter, and the option defaults to true

```lua

function fffff(aaa,bbb,ccc)
end

function ddddd(eeee,ffff,
    ggg,hhh)
end
```

After formatting:
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

This option means that any line of a continuous assignment statement that is separated by more than one space from the left symbol is aligned to the equal sign using the principle of least alignment, and the default value of this option is true

```lua

local t  = 123
local cccc = 456
cccc = 321
-- this is a comment
eeeeeeeee = 654 -- this is a comment2

-- no continuous
local c =132
```

After formatting:

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

This option indicates whether the items in the table are allowed to snap to equal signs, and the rules for alignment are similar to the above option, and the default value of the option is true

```lua
local t = {
    aadddd  = 123,
    bbb =456,
    ddd =789
}
```

Format:

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

This option indicates whether to align the branches of the if statement, including branches composed of 'and' and 'or', and defaults to false

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

After formatting:
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

This option indicates whether to align contiguous array items in the table, which defaults to true

The effect of formatting is as follows:
```lua
local t = {
    { "fwfwf", njqoifjiowiof, 121313 },
    { "fwfw",  fjwofw,        wngjwoigw },
    { a,       b,             c,        d },
    { 1,       2,             3 },
}
```

## Other indentation settings

### never_indent_before_if_condition

This option indicates that conditional expressions for if statements do not need to be indented if they span rows.

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
### never_indent_comment_on_if_branch

This option means that comments above if/elseif/else are no longer indented

## The line layout of the statement

The tool supports setting the line spacing between different statements, including if, while, repeat, for, do, local/assign, comment statements.

These option names and default values are:
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

All options support three value expressions:
- min(${n})
    
Indicates that the distance between the next statement is at least n lines
- keep

Indicates that the original line spacing is maintained with the next line
- fixed(${n})

The representation and the next line are fixed with n line spacing
- max(${n})
	
Represents up to n line spacing with the next line

## Line break

### break_all_list_when_line_exceed

This option means that when an expression with a list exceeds the row width setting, interrupt all, that is, break to each row, the default value is false, and it is currently only valid for tables

### auto_collapse_lines

This option means that expressions on the list should not be interrupted after evaluation, they will all collapse to the same row, the default value is false, and it is currently only valid for tables

## Preferences

### ignore_space_after_colon

This option indicates that formatting of blanks after the colon is ignored

### remove_call_expression_list_finish_comma

This option removes the comma at the end of the invocation expression (this is an extended syntax)

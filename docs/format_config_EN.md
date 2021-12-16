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

## keep_one_space_between_call_args_and_bracket

This option indicates whether there is a space between the parameters of the function call expression and the left and right parentheses. The default value of the option is false

Before formatting: 

```lua
helloWorld(aaa,bbb,ccc,
eee,ddd)

print(123,456)

```

after formatting：

```lua
--keep_one_space_between_call_args_and_bracket = true

helloWorld( aaa, bbb, ccc,
    eee, ddd )

print( 123, 456 )

```

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

## continuous_assign_statement_align_to_equal_sign

This option means that if the first line of the continuous assignment statement is more than one space between the equal sign and the left symbol, it will be aligned to the largest equal sign of the continuous assignment statement. The default value of this option is true.

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
```

All options support three value expressions:
- minLine:${n}
    
    Indicates that there is at least n lines between the next sentence
- keepLine

    Means to maintain the original line spacing with the next line
- keepLine:${n}

    Means to maintain n line spacing with the next line 

# Introduction to formatting behavior 

## Basic Statement

The algorithm itself will use more reasonable principles to do basic code layout optimization:
* All logos/keywords/literal expressions usually keep a space or relative line spacing with the next element
* Binocular/monocular operators will keep a space with the operand or keep relative line spacing, there is no space between the comma and the previous element and the next element keeps a space
* Short statement blocks can (but not necessarily) remain on the same line. This principle is valid for all function/do/while/repeat/if statements
* The algorithm will not automatically interrupt a single-line statement that is too long. The algorithm seldom actively wraps. The algorithm will try to ensure that the line spacing of all elements is the same as before.
* The algorithm does not remove any visible elements, such as semicolons. Do not format if there is a syntax error
* The algorithm will not typeset the content of the comment, for example, all the parts of the long comment that are considered to be comments will not increase the indentation 

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

Will be formatted as:

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

## Continuous assignment statement 

For consecutive assignments or local statements, when the algorithm recognizes that the code tries to align to the equal sign, it will typeset in the manner of aligning to the equal sign. The basic principle of identifying alignment is:
    
* For consecutive assignment/local/comment statements, when the equal sign of the first assignment/local statement is more than 1 space away from the element to its left, the consecutive assignment/local statement will be aligned to the equal sign.
* The definition of continuous alignment here is: no more than 2 lines between statements
* Alignment will be aligned to the furthest equal sign 

```lua
local ttt  = 123 --first
cd        = 345 --second
```

格式化后：

```lua
local ttt  = 123 --first
cd         = 345 --second
```

## Table layout 

For common table expressions, the default formatting method is:
* The field in the table will be spaced a blank from the braces of the table or keep relative line spacing
* Different fields in the table will keep their relative positions unchanged
* If all the fields are in different rows and are in the form of key = value, when different fields try to align to the equal sign, the algorithm will align to the equal sign
* The basic principle of trying to align to the equal sign is key = value/comment field. When the distance between the equal sign of the first line of field and the element to its left is greater than 1 space, the key = value field in the table will be aligned to the equal sign 

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

## Long expression list 

If the long expression list and the long expression have more than one line, except for the first line, the remaining lines will be indented by one continuation line:

For example: 

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

## Statements containing statement blocks

For if statement, repeat statement, while statement, do statement, for statement and function definition statement they all have the following formatting characteristics:

* When the statement containing the statement block is only one line, the algorithm will keep them in one line
* For statements that usually contain statement blocks, there will be one more indentation in the statement block
* The statement containing the statement block and the next statement are usually separated by at least 1 line

For example: 

```lua
do return end
function f(x,y) return x<y  end
function fff()
local t =123
end
```

Will be formatted as: 

```lua
do return end

function f(x, y) return x < y end

function fff()
    local t = 123
end

```

## Function definition parameters and function call parameters

The definition parameters of function definition statements have the following principles:
* By default, if the definition parameter of the function wraps, it will be aligned to the first parameter after the wrap

For example: 

```lua
function ffff(a, b, c,
    callback)
end
```

After formatting: 

```lua
function ffff(a, b, c,
              callback)
end

```

The parameters of function call have the following principles:
* If the function has multiple parameters, if the parameter list itself changes lines, except for the first line, the remaining lines will be indented by one more continuation line
* If the function call is a single parameter without parentheses, there will be a space between the function identifier and the parameter

For example: 

```lua
print(aaa, bbbb, eeee, ffff,
ggggg, hhhhh,
lllll)
require "code_format"
```
After formatting: 

```lua
print(aaa, bbbb, eeee, ffff,
    ggggg, hhhhh,
    lllll)
require "code_format"
```

## Comment statements and inline comments

Comment statements are divided into long comments and single-line short comments and inline comments
* When the comment statement is formatted, the short comment will not make any changes, keeping the same line spacing as the next statement, while the large blank text of the long comment is considered part of the comment, so no changes will be made, and the same Same line spacing in the next statement
* Inline comments, using the principle of keeping a space with the previous text, if the next element is still on the same line, it will also keep a space 

```lua
--[[
    
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


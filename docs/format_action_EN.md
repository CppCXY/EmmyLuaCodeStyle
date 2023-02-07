# Introduction to formatting behavior

The formatter is highly configurable, supports simple extended syntax, has a variety of styles, and is extremely performant. Suitable for formatting configuration files, formatting large code bases, etc. Suitable for a unified code style within the project, or individual developers with special preferences.

## Basic formatting

The basic formatting principle of the code is mainly to add basic whitespace on both sides of the symbol, achieve correct indentation for the statement block, set the appropriate spacing between different statements, keep the token in the sequential position will not change, and clean up the excess space at the end of the line.

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

will be formatted as:
```lua
local t = 123 --local
aaa, bbbb = 1123, 2342
local f = function()
end
local d = {
    -- comment
    aa = 213, --comment
};
local e = a + b + c + d
function fff(a, b, c)
end

function ff2()
    --[[
    long comment
]]
end
```

## Alignment

On top of basic requirements, formatting tools provide satisfaction for advanced aesthetic needs, and code alignment is an important component.


```lua
local ttt  = 123 --first
cd        = 345 --second


local d = {
    aa  =123,
    bbbb = 4353,
    eee  = 131231,
}

local c = {
    { aaaaa, bbbb, ccc },
    { 1, 2, 3 }
}
```

will be formatted as:

```lua
local ttt = 123 --first
cd        = 345 --second


local d = {
    aa   = 123,
    bbbb = 4353,
    eee  = 131231,
}

local c = {
    { aaaaa, bbbb, ccc },
    { 1,     2,    3 }
}
```

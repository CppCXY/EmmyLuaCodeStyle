# 📚 Formatting behavior

## Basic formatting

EmmyLuaCodeStyle applies a consistent set of rules to ensure readability and maintainability of Lua source code. Its core formatting principles include:

- Adding appropriate whitespace around symbols
- Ensuring correct indentation for statement blocks
- Maintaining consistent spacing between statements
- Preserving token order to avoid semantic changes
- Removing trailing whitespace at the end of lines

These rules guarantee a uniform code structure across projects while respecting the logical flow of Lua syntax.

**Example:**

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
    long description.
]]
end
```

**Formatted output:**

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
    long description.
]]
end
```

### Alignment

Beyond basic formatting, it supports advanced alignment features to improve clarity and readability. Adequate alignment helps developers easily scan related code elements and maintain a clean, consistent structure.

**Example:**

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

**Formatted output:**

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

The formatter aligns assignment operators, table fields, and array elements in a way that balances readability with minimal unnecessary whitespace. Alignment can be configured or disabled entirely, per project coding conventions.

### Line wrapping

EmmyLua provides flexible line wrapping rules through configuration options such as `max_line_length = 120` and `continuation_indent = 4`. Long expressions, function parameters, and table constructors are automatically wrapped according to the configurable maximum line length.

**Example:**

```lua
local function longFunctionNameWithManyParameters(param1, param2, param3, param4, param5, param6)
    -- function body
end
```

**Formatted output:**

```lua
local function longFunctionName(
    with_many_parameters, param_two, param_three,
    configurable, continuation_indent
)
    -- function body
end
```

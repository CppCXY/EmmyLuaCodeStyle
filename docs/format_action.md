# 格式化行为介绍

该格式化工具具有高度可配置，可支持简单扩展语法，风格多样，性能卓绝等特点。适用于格式化配置文件, 格式化大型代码库等。适合项目内统一代码风格，或者具有特别的喜好个人开发者。

## 基本格式化

代码的基本格式化原则主要是为符号两边增设基本的空白，为语句块实现正确的缩进，为不同语句之间设定合适的间距，保持token在顺序位置上不会改变，清理行尾多余的空白。

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

会被格式化为：
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

## 对齐

在基本需求之上格式化工具提供对进阶的审美需求的满足，代码对齐是很重要的组成部分。

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

格式化后：

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

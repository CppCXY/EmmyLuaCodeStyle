# 格式化行为介绍

该格式化工具具有高度可配置，可支持简单扩展语法，风格多样，性能卓绝等特点。适用于格式化配置文件, 格式化大型代码库等。适合项目内统一代码风格，或者具有特别的喜好个人开发者。

格式化结果符合基本审美，格式化效果出众，格式化速度一骑绝尘。

## 基本格式化

代码的基本格式化主要是为符号两边增设基本的空白，为语句块实现正确的缩进，为不同语句之间设定合适的间距。

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

代码对齐也是很重要的需求，连续赋值语句，连续的键值对表项，连续的数组类表都可以对齐

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

## if语句对齐

lua没有switch语句，一般都是通过if/elseif做一个匹配列表，如果喜欢的话可以设置类似于switch-case的对齐方式。

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
if     aa.isDDDD()
and    bb == fwfwfw
or     hi == 123 then
    print(1313)
elseif cc == 123
or     dd == 13131 and ddd == 123 then
    local ccc = 123
end
```

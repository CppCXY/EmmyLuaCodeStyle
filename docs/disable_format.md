# 格式化屏蔽

通常项目中存在一些不宜格式化的文件，或者在一个文件中一些语句的格式化需要例外的手动处理，对于这样的情况，该算法库依然提供一定的支持

## 使用---@format

遵循emmylua doc的方式，我实现了通过分析注释屏蔽格式化的方式。

在需要屏蔽的语句上方加上`---@format disable-next`即可屏蔽该语句内的格式化。

例如：
```lua
---@format disable-next
local t =123

```

如果某语句块需要屏蔽则加上`---@format disable`即可屏蔽该语句块内的格式化.

例如:
```lua
---@format disable

local d = 123
local cc=2313
local ccc =13

```

## 注意

1. 无论是否屏蔽格式化，被format注解屏蔽格式化的语句的缩进依然会被格式化，
2. 屏蔽格式化的单位是语句，不是表达式，如果format注解写为内联注释，则屏蔽不会生效

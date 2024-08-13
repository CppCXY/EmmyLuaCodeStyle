##  概述

命名风格检查支持基于基本命名法的检查和正则表达式检查:
* snake-case
* camel-case
* pascal-case
* upper-snake-case
* pattern

## 配置

开启命名风格检查需要在settings添加配置:
```json
"emmylua.lint.nameStyle": true
```

在vscode的setting中增加配置项`emmylua.name.config`, 可如下填写:
```json
  "emmylua.name.config": {
      "local_name_style": "snake_case"
  },
```
可配置的项有:
* local_name_style
* function_param_name_style
* function_name_style
* local_function_name_style
* table_field_name_style
* global_variable_name_style
* module_name_style
* require_module_name_style
* class_name_style
* const_variable_name_style
* module_local_name_style (没有额外作用域的变量，如果没有设置默认值，则将其默认值设置为 `local_name_style`)

每一个可配置项的格式都是相同的, 每个可配置项可配置的值支持如下格式:
* 单字符串 例如: 
```json
"local_name_style": "snake_case"
```

单字符串支持的值有: `snake_case`, `pascal_case`, `upper_snake_case`, `camel_case`

* 字符串数组, 例如:
```json
"local_name_style": [
    "snake_case",
    "upper_snake_case"
]
```

* 忽略列表

忽略列表的一般形式是:
```json
"local_name_style": {
    "type": "ignore", 
    "param": ["m", "M", "Do"]
}
```

* 正则表达式

正则语法为javascript正则语法.
正则表达式的一般形式是:
```json
"local_name_style": {
    "type" : "pattern",
    "param": "uuu*"
}
```

正则表达式支持捕获组后再匹配基本命名法:
```json
"local_name_style": {
    "type" : "pattern",
    "param": "m_(\\w+)",
    "$1": "camel_case"
}
```

其中`"$1": "camel_case"`表示第一个捕获组的内容需要匹配 `camel_case` 命名法

* 支持表结构和字符串的混合数组 例如:

```json
"local_name_style": [
    "snake_case",
    "pascal_case",
    "camel_case",
    {
        "type" : "pattern",
        "param": "m_(\\w+)",
        "$1": "camel_case"
    }
]
```

## 可配置项示例

以下列表显示了每个可配置项针对的元素，并提供了一个小代码示例，同时列出了默认配置。

### `local_name_style`

> 默认值：`snake_case`

在以下示例中，检查的名称是 `result`。请注意，由于作用域的原因，`some_value` 是由 `module_local_name_style` 检查的。

```Lua
local some_value = 42

local function sum(first_value, second_value)
    local result = first_value + second_value
    return result
end
```

### `function_param_name_style`

> 默认值：`snake_case`

在以下示例中，检查的名称是 `first_value` 和 `second_value`。

```Lua
local function sum(first_value, second_value)
    local result = first_value + second_value
    return result
end
```

### `function_name_style`

> 默认值：`snake_case`

在以下示例中，检查的名称是 `some_action`。

```Lua
function some_action() 
    return 0
end
```

### `local_function_name_style`

> 默认值：`snake_case`

在以下示例中，检查的名称是 `calculate_square`。

```Lua
local function calculate_square(x)
  return x * x
end
```

### `table_field_name_style`

> 默认值：`snake_case`

在以下示例中，检查的名称是 `first_name` 和 `last_name`。

```Lua
local person = {}
person.first_name = "John"
person.last_name = "Doe"
```

### `global_variable_name_style`

> 默认值：`snake_case` 和 `upper_snake_case`

在以下示例中，检查的名称是 `MAX_ELEMENTS`。

```Lua
MAX_ELEMENTS = 100
```

### `module_name_style`

> 默认值：`snake_case`

在以下示例中，检查的名称是 `my_module`。

```Lua
local my_module = {}

-- [...]

return my_module
```

### `require_module_name_style`

> 默认值：`snake_case` 和 `pascal_case`

在以下示例中，检查的名称是 `util_module`。

```Lua
local util_module = require("util_module")
local data_module = import("data_module")
```

### `class_name_style`

> 默认值：`snake_case` 和 `pascal_case`

在以下示例中，检查的名称是 `person`。

```Lua
local person = Class()
```

### `const_variable_name_style`

> 默认值：`snake_case` 和 `upper_snake_case`

在以下示例中，检查的名称是 `PI`。

```Lua
local PI <const> = 3.14
```

### `module_local_name_style`

> 默认值：回退到当前的 `local_name_style` 配置

在以下示例中，检查的名称是 `some_value` 和 `data`。

```Lua
local some_value = 42
local data = {}

local function sum(first_value, second_value)
    local result = first_value + second_value
    return result
end
```

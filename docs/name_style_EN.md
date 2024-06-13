## Overview

Naming style checking supports checking based on basic nomenclature and regular expression checking:
* snake-case
* camel-case
* pascal-case
* upper-snake-case
* pattern

## Configuration

To enable naming style checking, you need to add the following configuration in the settings:
```json
"emmylua.lint.nameStyle": true
```

Add the configuration item `emmylua.name.config` in the setting of vscode, which can be filled in as follows:
```json
   "emmylua.name.config": {
       "local_name_style": "snake_case"
   },
```
The configurable items are:
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
* module_local_name_style (variables without additional scope, setting defaults to the one of `local_name_style` if not set)

The format of each configurable item is the same, and the configurable value of each configurable item supports the following formats:
* Single string Example:
```json
"local_name_style": "snake_case"
```

Supported values for single string are: `snake_case`, `pascal_case`, `upper_snake_case`, `camel_case`

* String array, for example:
```json
"local_name_style": [
     "snake_case",
     "upper_snake_case"
]
```

* ignore list

The general form of an ignore list is:
```json
"local_name_style": {
     "type": "ignore",
     "param": ["m", "M", "Do"]
}
```

* regular expression

The regular syntax is javascript regular syntax.
The general form of a regular expression is:
```json
"local_name_style": {
     "type" : "pattern",
     "param": "uuu*"
}
```

Regular expressions support capture groups and then match basic nomenclature:
```json
"local_name_style": {
     "type" : "pattern",
     "param": "m_(\\w+)",
     "$1": "camel_case"
}
```

Where `"$1": "camel_case"` means that the content of the first capture group needs to match the `camel_case` nomenclature

* Supports mixed arrays of table structures and strings

For example:

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

## Examples for the configurable items

The following list shows which elements each configurable item targets with a small code example, and lists the default configuration.

### `local_name_style`

> Default: `snake_case`

Checked name in the following example is `result`. Note that
`some_value` is checked by `module_local_name_style` because of its scope.

```Lua
local some_value = 42

local function sum(first_value, second_value)
    local result = first_value + second_value
    return result
end
```

### `function_param_name_style`

> Default: `snake_case`

Checked names in the following example are `first_value` and `second_value`.

```Lua
local function sum(first_value, second_value)
    local result = first_value + second_value
    return result
end
```

### `function_name_style`

> Default: `snake_case`

Checked name in the following example is `some_action`.

```Lua
function some_action() 
    return 0
end
```

### `local_function_name_style`

> Default: `snake_case`

Checked name in the following example is `calculate_square`.

```Lua
local function calculate_square(x)
  return x * x
end
```

### `table_field_name_style`

> Default: `snake_case`

Checked names in the following example are `first_name` and `last_name`.

```Lua
local person = {}
person.first_name = "John"
person.last_name = "Doe"
```

### `global_variable_name_style`

> Default: `snake_case` and `upper_snake_case`

Checked name in the following example is `MAX_ELEMENTS`.

```Lua
MAX_ELEMENTS = 100
```

### `module_name_style`

> Default: `snake_case`

Checked name in the following example is `my_module`.

```Lua
local my_module = {}

-- [...]

return my_module
```

### `require_module_name_style`

> Default: `snake_case` and `pascal_case`

Checked name in the following example is `util_module`.

```Lua
local util_module = require("util_module")
local data_module = import("data_module")
```

### `class_name_style`

> Default: `snake_case` and `pascal_case`

Checked name in the following example is `person`.

```Lua
local person = Class()
```

### `const_variable_name_style`

> Default: `snake_case` and `upper_snake_case`

Checked name in the following example is `PI`.

```Lua
local PI <const> = 3.14
```

### `module_local_name_style`

> Default: Fallback on current `local_name_style` configuration

Checked names in the following example are `some_value` and `data`.

```Lua
local some_value = 42
local data = {}

local function sum(first_value, second_value)
    local result = first_value + second_value
    return result
end
```

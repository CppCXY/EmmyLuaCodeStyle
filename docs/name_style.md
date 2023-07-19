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


## Overview

Naming style checking supports checking based on basic nomenclature and regular expression checking:
* snake-case
* camel-case
* pascal-case
* upper-snake-case
* pattern

## configuration

To enable naming style checking, you need to add configuration in settings:
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

* Supports mixed arrays of table structures and strings For example:

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
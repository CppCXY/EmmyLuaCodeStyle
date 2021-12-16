# Diagnostic options

Most of the formatting options have diagnostic attributes. In addition, there are additional options for diagnostics. These options do not affect formatting.
## enable_check_codestyle

Whether to enable code diagnosis The optional value is true/false.

## max_line_length

Represents the maximum line width detection. This option does not affect formatting. The default value is 120.

## insert_final_newline

This option indicates whether the Lua document should end with a new line. The name of this option comes from the standard definition of editorconfig. The default value is true. The code formatting algorithm must end with a new line, so this option is only used for diagnosis.

# Naming style check

The algorithm supports basic style checking for the name definition, and the more complex verification logic can be opened after the plug-in system is designed. All check logics can be represented by'|' or' operation. The basic check logic includes:
* off do nothing
* camel_case little camel case nomenclature
* pascal_case big hump nomenclature
* snake_case snake nomenclature
* upper_snake_case snake nomenclature in all uppercase

The complex verification logic is expressed in the form of a function:
1. same(arg1, arg2) This form means that the form of arg2 is the same as arg1. The only variables supported by arg2 are:
    * snake_case in a snake-like way
    * pascal_case is in the form of big hump nomenclature
    * camel_case in the form of small camel case nomenclature
    
    The methods supported by arg1 are:

    * filename means the same as the current file name, the same way is not necessarily congruent, it can be in the form of arg2
    * first_param means the same as the first parameter of the related calling function, and it is also allowed to act in the form specified by arg2
    * String constant means equal to a string constant, such as same('_M'), when arg1 is a string constant, the arg2 parameter is invalid

## enable_name_style_check

This option is false by default, which means whether to enable naming style checking

## local_name_define_style

This option indicates what naming rules the names in the local name definition list should conform to. The default value is snake_case

## function_param_name_style

This option indicates that the default value of the parameter naming rule during function definition is snake_case

## function_name_define_style

This option indicates the naming rules for function definitions. The function definition referred to here only contains the following forms:
* function fff() end
* function ttt:ffff() end

The default value of the option is snake_case

## local_function_name_define_style

This option indicates the naming rule when the local function is defined. The only supported form here is: local function fff() end

The default value of the option is snake_case

## table_field_name_define_style

This option indicates the naming rule of the field of the table, and the classification may be evolved later. The currently supported forms are: 

```lua
local t = { aa = 1312}

t.bbb = 123
```

The default value is snake_case, but the names of all meta-methods are not within the rules.

## global_variable_name_define_style

Indicates the name definition style of global variables. The default value is snake_case|upper_snake_case

## module_name_define_style

Indicates the name definition style of the module, and the identification rules for module variables are:
Take the name of any local definition in the file scope as the first parameter of the return statement in the file scope.

The default option is same('m')|same(filename, snake_case)

## require_module_name_style

Indicates the name defined in the import statement of the module. The statement recognition principle is:
Shaped like 
```lua
local m = require "aaa.bbb.ccc" 
```
The m in the lua statement, the supported functions currently include require and import


The default value is same(first_param, snake_case) 

## class_name_define_style

Represents the naming rules of class definitions. The recognition principles of class definitions are:
Shaped like 
```lua
local c = class "c"
```
The c in the lua statement, the supported functions currently include Class and class

The default value is same(filename, snake_case) 

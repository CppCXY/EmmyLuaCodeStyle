# 诊断选项

大部分的格式化选项会有诊断的属性，除此以外诊断有额外的选项，这些选项并不影响格式化。
## enable_check_codestyle

是否开启代码诊断 可选值为 true/false。

## max_line_length

代表最大行宽检测，该选项不会影响格式化，默认值是120。

## insert_final_newline

该选项表示lua文档是否应该以新行结束，该选项的名称来自于editorconfig的标准定义默认值为true，代码格式化算法必然会以新行结束，所以该选项只是用于诊断。

# 命名风格检查 

算法支持对名称定义处实现基本的风格检查，较为复杂的校验逻辑在设计出插件系统之后可以开放。所有校验逻辑之间可以用'|'表示'或'运算，基本的校验逻辑包括:
* off 什么都不做
* camel_case 小驼峰命名法
* pascal_case 大驼峰命名法
* snake_case 蛇形命名法
* upper_snake_case 全大写的蛇形命名法

复杂的校验逻辑以函数的形式表示:
1. same(arg1, arg2) 该形式表示以arg2的形式和arg1相同，arg2支持的变量仅有：
    * snake_case 以蛇形的方式
    * pascal_case 以大驼峰命名法的形式
    * camel_case 以小驼峰命名法的形式
    
    arg1支持的方式有:

    * filename 表示和当前文件名相同，相同的方式不一定是全等，可以是以arg2的形式
    * first_param 表示和相关的调用函数的第一个参数相同，也是允许以arg2规定的形式作用
    * 字符串常量 表示和某个字符串常量相等例如 same('_M'),当arg1为字符串常量时，arg2参数无效

## enable_name_style_check

该选项默认为false，表示是否开启命名风格检查

## local_name_define_style

该选项表示local的名称定义列表中的名称应该符合什么样的命名规则，默认值是snake_case

## function_param_name_style

该选项表示函数定义时的参数命名规则默认值是snake_case

## function_name_define_style

该选项表示函数定义时的命名规则，此处所指的函数定义仅仅包含以下形式:
* function fff() end
* function ttt:ffff() end

选项默认值是 snake_case

## local_function_name_define_style

该选项表示local函数定义时的的命名规则，此处唯一支持形式是：local function fff() end

选项默认值是snake_case

## table_field_name_define_style

该选项表示表的field的命名规则，后面可能会演化分类，目前支持的形式是：

```lua
local t = { aa = 1312}

t.bbb = 123
```

默认值是是snake_case，但是所有元方法的名称不在规则限制内。

## global_variable_name_define_style

表示全局变量的名称定义风格默认值是 snake_case|upper_snake_case

## module_name_define_style

表示模块的名称定义风格，对模块变量的识别规则是：
以在文件作用域中任意local定义的名称作为文件作用域中的return语句的第一个参数。

默认选项是 same('m')|same(filename, snake_case)

## require_module_name_style

表示模块的导入语句中定义的名称，语句的识别原则是：
形如
```lua
local m = require "aaa.bbb.ccc" 
```
的lua语句中的m，支持的函数目前有require和import



默认值是 same(first_param, snake_case)

## class_name_define_style

表示类定义的命名规则，类定义的识别原则是：
形如
```lua
local c = class "c"
```
的lua语句中的c，支持的函数目前有Class和class

默认值是 same(filename, snake_case)

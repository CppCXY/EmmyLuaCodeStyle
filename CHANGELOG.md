# Change Log

[English Change Log](CHANGELOG_EN.md)

# 1.3.1

`FIX` 修复了`space_before_inline_comment=2`对大括号之后的注释不起作用的BUG

`NEW` 新增选项`line_space_around_block`表示语句块开始和末尾保留的空行数, 该选项默认值为`fixed(1)`, 表示语句块不保留前面和后面的空行. 其可选的值范围是:
`keep`, `max(n)`, `min(n)`, `fixed(n)`, 该选项无法作用于最外层的代码块.


# 1.3.0

`REFACTOR` 重写了分词算法, 和相关的拼写检查

`FIX` 修复了错误的添加表项的尾部分隔符的问题

`NEW` 支持选项`end_statement_with_semicolon`, 现在你可以设置语句是否保留行尾分号, 或者强行保持分号, 该选项提供对应诊断消息, 感谢`@AndreaWalchshoferSCCH`

`NEW` 支持选项`keep_indents_on_empty_lines`, 这个选项主要是intellij平台通常会有默认的一个选项保持空行缩进, 感谢`@qq792453582`


# 1.2.2

`FIX` 当前行存在unicode字符时,放弃对齐行为

`NEW` 支持非标准符号

# 1.2.0

`NEW` 支持选项`break_before_braces`

`NEW` 支持设置`table_separator_style`

`NEW` 命令行工具支持范围格式化

`NEW` 支持了intellij平台, Intellij-EmmyLua的用户在2023.1以上的IDE可以安装本插件

# 1.1.5

`NEW` 支持通过设置选项 `align_continuous_similar_call_args=true` 对齐相似函数的参数列表

## 1.1.4

`FIX` 修复拼写检查在字符串中少一位的BUG

`CHANGE` 命名风格检查的camel_case 和 snake_case 不再兼容下划线前缀, 有相关需求的请使用正则表达式

`FIX` 修复命名风格检查过早退出检查循环的BUG

`FIX` 修复命名风格检查对空节点的错误检查, 和对table field的检查

`Feature Request` 如果有更多合理的格式化样式的设置需求可以直接发起issue

`Develop` 最近特性加入比较慢, 主要是我在全力开发新的语言服务

## 1.1.3

`NEW` 功能 `命名风格检查` 正式支持配置.

`CHANGE` `local empty = function() end` 格式化时不会再换行

`NEW` 支持形如 `local d; Init()` 的排版方式

`NEW` 支持链式表达式对齐

## 1.1.2

`NEW` 新选项 `align_continuous_line_space`

`NEW` 还有其他更新, 但是我忘了

## 1.1.1

`NEW` 优化type format

`FIX` type format 不会清理`---@format disable` 和`---@format disable-next`之下的内容了

## 1.1.0

`FIX` 修复许多BUG 


`NEW` 重新支持`---@format disable` 和 `---@format disable-next`

## 1.0.9

`FIX` 修复缩进检查的一些BUG

## 1.0.8

`NEW` 支持缩进检查

`NEW` 修复一些BUG

## 1.0.7

`FIX` fix #89 #90

## 1.0.6

`NEW` 支持内联注释对齐

## 1.0.5

`FIX` 修复文件增量更新算法的BUG

## 1.0.4

`Upgrade` 试图修复vscode-languageclient带来的依赖错误

## 1.0.3

`Upgrade` 试图修复插件无法激活的问题, 更新所有依赖库

`FIX` 修复一个格式化行为.

`NEW` 选项`space_before_function_call_single_arg`支持其他值always/only_string/only_table/none

## 1.0.2

`FIX` 修复lineIndex错误导致的Unicode字符BUG

## 1.0.0

`EmmyLuaCodeStyle` 经过一年的更新现在进入正式版

`Refactor` 大部分代码重写, 数据结构重新设计

`Refactor` 大部分选项被重新命名, 新增了一批选项, 移除了一批选项

`Performance` 得益于从头开始重新设计, 格式化性能提高三倍, 在windows上一般的机器上格式化`10w`行代码仅需1到2秒 

## 0.18.0

`FIX` typeformat不会在字符串或者长注释内部做格式化行为

以下由[@obszczymucha](https://github.com/obszczymucha)实现:

`NEW` 支持选项
* `space_inside_function_call_parentheses`
* `space_inside_function_param_list_parentheses`
* `space_inside_square_brackets`


## 0.17.2

`NEW` typeformat会智能填补表项之后的逗号

`NEW` 支持选项`table_separator_style` 可选值为`none/comma/semicolon`

`NEW` 支持选项`trailing_table_separator` 可选值为 `keep/never/always/smart`

## 0.16.1

`FIX` 优化typeformat体验

## 0.16.0

`NEW` 开启typeformat后，键入'\n'时会自动补完缺失的end

## 0.15.0

`CHANGE` 改变代码风格检查时提示的文字

## 0.14.1

`FIX` 修复因为设置`insert_final_newline = false`和开启typeOnFormat特性导致回车无效得BUG

## 0.14.0

`CHANGE` 配置项`insert_final_newline`现在符合.editorconfig规定

`CHANGE` 支持`break` 和 `goto` 语句后的分号

`CHANGE` 格式化将移除空行分号，移除if语句，while语句，repeat语句后的分号

`NEW` 支持通过注解部分屏蔽格式化，详细看[disable-format](https://github.com/CppCXY/EmmyLuaCodeStyle/blob/master/docs/disable_format.md)


## 0.13.0

`CHANGE` 放宽对齐调用表达式的限制

`NEW` 支持扩展语法，允许 '`' 表达的字符串

`NEW` 新增选项`space_before_function_open_parenthesis` 表示函数和其括号之间保持一个空格(包括函数定义，和函数调用，以及闭包表达式)

`NEW` 新增选项`space_before_open_square_bracket` 表示在索引表达式中的中括号和左边符号相距一个空格

`FIX` 修复因为支持`t[ [[1111]] .. 1 ]`的排版而错误将`t[t[1]]`格式化为`t[ t[1] ]`的问题


## 0.12.0

`NEW` 当代码超过指定行宽时会自动折行，该行为可能会产生不如意的代码排版。这时候建议ctrl z之后手动排版再格式化。

`FIX` 修复索引表达式中涉及长字符串时，格式化导致的语法错误

## 0.11.4

`CHANGE` 更新基本词典

`NEW` 支持字符串中的拼写检查

## 0.11.3

`CHANGE` 更新词典

`CHANGE` 语言服务按lsp3.17版本重新设计, 诊断采用lsp规定的方式诊断。

## 0.11.2

`CHANGE` 更新lua常用词词典 

## 0.11.0

`CHANGE` 重构了语言服务的代码组织，暂时移除插件对补全的影响，插件不在代码补全时提示模块了

`CHANGE` 优化了模块诊断时提示的内容

`NEW` 提供针对lua优化的标识符单词拼写检查算法

`NEW` 提供了新的格式化选项:
* `remove_empty_header_and_footer_lines_in_function` 允许移除函数的上面和下面多余空行
* `remove_expression_list_finish_comma` 允许移除函数调用时最后一个逗号(这本身是一个语法错误)

## 0.10.3

`NEW` 命令行工具支持使用通配符过滤

## 0.10.2

`CHANGE` 默认情况下namedef和attribute会有一个空格的距离

`NEW` 行布局支持使用`maxLine:$n`指定与下一行的最大行距

`NEW` 行宽允许设定为unset

`CHANGE` 函数调用的排版方式变更，更符合lua中的一些使用习惯


## 0.10.0

`NEW` vscode插件端实现配置EmmyluaCodeStyle开关`模块诊断`,`代码风格诊断`, `模块补全`.

`NEW` 行距诊断换一种表现方式

`NEW` 支持通过设置`align_chained_expression_statement=true`使链式表达式对齐到第一个'.'或者':' 

`NEW` 支持通过设置`if_branch_comments_after_block_no_indent`允许else或者elseif之上的注释对齐到关键词上


## 0.9.7

`IMPL` [#31](https://github.com/CppCXY/EmmyLuaCodeStyle/issues/31)

`IMPL` [#30](https://github.com/CppCXY/EmmyLuaCodeStyle/issues/30)

1. 格式化会移除注释的尾部空白
2. 通过选项`if_branch_comments_after_block_no_indent = true` 允许在else/elseif分支之上的注释不会缩进到语句块内部

## 0.9.6

命令行工具帮助打印更规范
## 0.9.5

命令行工具在指定单输入文件时可以指定工作区，这可以帮助detect config。

## 0.9.4

修复命令行工具无法输出到文件的bug

## 0.9.2

命令行工具支持批量格式化和批量诊断

## 0.9.0

1. 命令行工具现在会自动寻找并使用最近的editorconfig配置
2. 长字符串和长注释会采用配置的行尾格式化

## 0.8.7

重写命令行工具

## 0.8.5

修复一个bug:纯注释文件会被直接清空

## 0.8.4

`FIX` [#27](https://github.com/CppCXY/EmmyLuaCodeStyle/issues/27)
## 0.8.3

`FIX` [#26](https://github.com/CppCXY/EmmyLuaCodeStyle/issues/26)

## 0.8.2

`FIX` [#25](https://github.com/CppCXY/EmmyLuaCodeStyle/issues/25)

## 0.8.1

`FIX` [#24](https://github.com/CppCXY/EmmyLuaCodeStyle/issues/24)

## 0.8.0

`Resolve` [#20](https://github.com/CppCXY/EmmyLuaCodeStyle/issues/20)

`Resolve` [#17](https://github.com/CppCXY/EmmyLuaCodeStyle/issues/17)

`Resolve` [#16](https://github.com/CppCXY/EmmyLuaCodeStyle/issues/16)

`Resolve` [#15](https://github.com/CppCXY/EmmyLuaCodeStyle/issues/15)

主要更新的特性为允许设定单双引号风格，对齐时采用最小对齐,允许移除函数调用的括号

## 0.7.0

1. 修复存在某些语法错误的情况下仍然格式化的问题
2. 支持if的条件表达式互相对齐
3. table表达式内支持不同的连续有不同的对齐
4. 形如t[#t+1]可以无空白包含
5. sumneko_lua 集成了该插件的主要功能
    
    
## 0.6.1

更新文档，更新editorconfig模板


## 0.6.0

1. 提高测试的覆盖率,每一个格式化选项都有专门的测试
2. 重构缩进算法,更好的支持使用tab作为缩进
3. 提供许多与缩进有关的格式化选项
4. 部分格式化选项重新命名了
5. 修复许多bug


## 0.5.1

修复linux下不可用的问题

## 0.5.0

将语言服务由单线程分离io线程和逻辑线程。io线程会尽可能快的读取消息，逻辑线程为单线程内无锁异步(不含asio自己控制的锁)
采用增量更新，单次发包量大幅度降低，语言服务采用原地增量更新算法(极少重新申请空间储存代码)
语言服务实现调度机制，代码诊断实现后端防抖，防抖区间暂定0.3秒。
大幅度提高插件性能，代码诊断不再成为性能瓶颈，在10万行以内的代码不会影响插件的正常使用

## 0.4.5

修复由于linux下编译器版本为gcc11导致在大部分linux环境下不可用的问题

## 0.4.4

修复枚举不一致
## 0.4.3

修复诊断bug

## 0.4.2

1. 修改格式化规则，连续调用表达式中如果存在单参数省略括号的表达形式，则该参数与下一个调用参数列表保持一个间距。
2. 修复一个识别错误的bug

## 0.4.1

提供模块的特殊export.rule规则，使用形式待考虑

## 0.4.0

提供auto import 功能

## 0.3.2

修复label导致格式化结果不稳定的bug

## 0.3.1

现在修改配置会立即刷新所有打开的文件了

## 0.3.0

修复许多bug，重构迭代算法，增加测试用例

## 0.2.6

降低插件体积，插件实现分系统分发不同的包。

## 0.2.5

修复一个连续调用函数时格式化错误的问题

## 0.2.4 

修复因为cin.readsome无法在linux和macosx上正确工作导致无法正确运行的问题，windows上仍然使用cin,cout
linux和macosx上使用asio封装过的接口

## 0.2.2

优化节点的内存占用，优化常驻内存，采用mimalloc优化内存分配性能，优化内存碎片。

## 0.2.1

修复内存方面的问题，降低内存消耗

## 0.2.0

1. 重写.editorconfig读取规则，支持editorconfig的基本匹配规则和所有基本选项
2. 支持基于lua语法特性和日常习惯的lua命名风格检测规则
3. 语言服务实现内采用异常提前中断没必要的解析

## 0.1.18
    
修复命名风格检查未能正确工作的bug

## 0.1.17

1. 修改配置选项名称，和editorconfig使用的名称保持一直（建议重新导入配置）

2. 支持检测是否以新行结束文本

3. 支持命名风格检测（默认不开启）

## 0.1.16

代码诊断本地化

## 0.1.15

修复bug

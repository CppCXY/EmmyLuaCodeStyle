# Change Log

# 1.3.0

`REFACTOR` rewrote the tokenization algorithm and related spell-checking.

`FIX` fixed the issue of adding incorrect trailing separators to table entries.

`NEW` added support for the option `end_statement_with_semicolon`. Now you can choose whether to keep the semicolon at the end of statements or enforce it. This option provides corresponding diagnostic messages. Thanks to `@AndreaWalchshoferSCCH`.

`NEW` added support for the option `keep_indents_on_empty_lines`. This option is mainly for IntelliJ platform, which usually has a default option to keep indentation on empty lines. Thanks to `@qq792453582`.

# 1.2.2

`FIX` abandon alignment behavior when there are Unicode characters in the current line.

`NEW` support non-standard symbols.

# 1.2.0

`NEW` support option `break_before_braces`

`NEW` supports setting `table_separator_style`

`NEW` command line tool supports range formatting

`NEW` supports the intellij platform, Intellij-EmmyLua users can install this plugin in IDEs above 2023.1

# 1.1.5

`NEW` supports aligning the argument lists of similar functions by setting the option `align_continuous_similar_call_args=true`

## 1.1.4

`FIX` Fix the bug that the spell check is missing one bit in the string

The camel_case and snake_case of the `CHANGE` naming style check are no longer compatible with underscore prefixes, please use regular expressions if you have related needs

`FIX` Fix the bug that the naming style check exits the checking loop prematurely

`FIX` fix naming style checking error checking for empty nodes, and checking for table fields

`Feature Request` If you have more reasonable formatting requirements, you can directly issue an issue

`Develop` The addition of features has been slow recently, mainly because I am working hard to develop new language services

## 1.1.3

`NEW` feature `naming style checking` officially supports configuration.

`CHANGE` `local empty = function() end` will no longer wrap when formatting

`NEW` supports typesetting like `local d; Init()`

`NEW` supports chained expression alignment

## 1.1.2

`NEW` new option `align_continuous_line_space`

`NEW` There are other updates, but I forgot

## 1.1.1

`NEW` optimize type format

`FIX` type format will not clear the content under `---@format disable` and `---@format disable-next`

## 1.1.0

`FIX` fix many bugs


`NEW` re-support `---@format disable` and `---@format disable-next`

## 1.0.9

`FIX` fix some bugs in indentation check

## 1.0.8

`NEW` supports indentation checking

`NEW` fix some bugs

## 1.0.7

`FIX` fix #89 #90

## 1.0.6

`NEW` supports inline comment alignment

## 1.0.5

`FIX` fix the BUG of file incremental update algorithm

## 1.0.4

`Upgrade` tries to fix dependency errors brought by vscode-languageclient

## 1.0.3

`Upgrade` tries to fix the problem that the plugin cannot be activated, updating all dependent libraries

`FIX` fixes a formatting behavior.

`NEW` option `space_before_function_call_single_arg` supports other values always/only_string/only_table/none

## 1.0.2

`FIX` fix the Unicode character BUG caused by lineIndex error

## 1.0.0

`EmmyLuaCodeStyle` is now in production after a year of updates

`Refactor` Most of the code was rewritten, and the data structure was redesigned

Most options of `Refactor` have been renamed, a number of options have been added, and a number of options have been removed

`Performance` thanks to the redesign from scratch, the formatting performance has been improved by three times, and it only takes 1 to 2 seconds to format `10w` lines of code on an average machine on windows

## 0.18.0

`FIX` typeformat does not do formatting inside strings or long comments

The following is implemented by [@obszczymucha](https://github.com/obszczymucha):

`NEW` support option
* `space_inside_function_call_parentheses`
* `space_inside_function_param_list_parentheses`
* `space_inside_square_brackets`


## 0.17.2

`NEW` typeformat will intelligently fill the comma after the entry

`NEW` supports the option `table_separator_style` and the optional values are `none/comma/semicolon`

`NEW` supports option `trailing_table_separator`, optional value is `keep/never/always/smart`

## 0.16.1

`FIX` optimize typeformat experience

## 0.16.0

`NEW` After typeformat is turned on, the missing end will be automatically completed when typing '\n'

## 0.15.0

`CHANGE` changes the text of the prompt when checking the code style

## 0.14.1

`FIX` Fix the bug that the carriage return is invalid due to setting `insert_final_newline = false` and enabling the typeOnFormat feature

## 0.14.0

`CHANGE` configuration item `insert_final_newline` now complies with .editorconfig

`CHANGE` supports semicolons after `break` and `goto` statements

`CHANGE` formatting will remove the semicolon of blank lines, remove the semicolon after the if statement, while statement, and repeat statement

`NEW` supports partially blocking formatting through annotations, see [disable-format](https://github.com/CppCXY/EmmyLuaCodeStyle/blob/master/docs/disable_format.md) for details


## 0.13.0

`CHANGE` relaxes restrictions on alignment call expressions

`NEW` supports extended syntax, allowing strings expressed by '`'

`NEW` new option `space_before_function_open_parentthesis` indicates that there should be a space between the function and its parentheses (including function definition, function call, and closure expression)

`NEW` new option `space_before_open_square_bracket` indicates that there is a space between the square bracket and the left symbol in the index expression

`FIX` Fix the problem that `t[t[1]]` is formatted as `t[ t[1] ]` by mistake because `t[ [[1111]] .. 1 ]` is supported


## 0.12.0

`NEW` automatically wraps lines when the code exceeds the specified line width, which may result in unsatisfactory code layout. At this time, it is recommended to manually typeset and format after ctrl z.

`FIX` fix syntax errors caused by formatting when long strings are involved in index expressions

## 0.11.4

`CHANGE` update the base dictionary

`NEW` supports spell checking in strings

## 0.11.3

`CHANGE` update dictionary

`CHANGE` The language service is redesigned according to the lsp3.17 version, and the diagnosis is made in the way specified by lsp.

## 0.11.2

`CHANGE` update lua common word dictionary

## 0.11.0

`CHANGE` refactored the code organization of the language service, temporarily removed the impact of plugins on completion, and plugins no longer prompt modules when code completion

`CHANGE` optimizes the prompt content when module diagnosis

`NEW` provides an identifier word spell checking algorithm optimized for lua

`NEW` provides new formatting options:
* `remove_empty_header_and_footer_lines_in_function` allows removing extra empty lines above and below the function
* `remove_expression_list_finish_comma` allows removing the last comma in a function call (which itself is a syntax error)

## 0.10.3

The `NEW` command line tool supports filtering using wildcards

## 0.10.2

`CHANGE` By default namedef and attribute will have a space distance

`NEW` line layout supports using `maxLine:$n` to specify the maximum line distance from the next line

`NEW` line width allows setting to unset

`CHANGE` The typesetting method of the function call has changed, which is more in line with some usage habits in lua


## 0.10.0

`NEW` The vscode plug-in implements the configuration of the EmmyluaCodeStyle switch `module diagnosis`, `code style diagnosis`, `module completion`.

`NEW` Line spacing diagnosis changed to a different way of expression

`NEW` supports aligning chained expressions to the first '.' or ':' by setting `align_chained_expression_statement=true`

`NEW` supports to allow comments above else or elseif to be aligned to keywords by setting `if_branch_comments_after_block_no_indent`


## 0.9.7

`IMPL` [#31](https://github.com/CppCXY/EmmyLuaCodeStyle/issues/31)

`IMPL` [#30](https://github.com/CppCXY/EmmyLuaCodeStyle/issues/30)

1. Formatting removes trailing whitespace from comments
2. By option `if_branch_comments_after_block_no_indent = true`, allow the comments above the else/elseif branch not to be indented inside the statement block

## 0.9.6

The command line tool helps to print more standardized
## 0.9.5

The command-line tool can specify a workspace when specifying a single input file, which can help detect config.

## 0.9.4

Fix the bug that the command line tool cannot output to the file

## 0.9.2

Command line tools support batch formatting and batch diagnostics

## 0.9.0

1. The command line tool will now automatically find and use the latest editorconfig configuration
2. Long strings and long comments will be formatted using the configured line endings

## 0.8.7

Rewrite the command line tool

## 0.8.5

Fix a bug: pure comment files will be cleared directly

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

The main updated feature is allowing to set the style of single and double quotes, using the minimum alignment when aligning, and allowing to remove the parentheses of function calls

## 0.7.0

1. Fix the problem of formatting even if there are some grammatical errors
2. The conditional expressions that support if are aligned with each other
3. Different alignments are supported in table expressions
4. Forms such as t[#t+1] can contain no blanks
5. sumneko_lua integrates the main functions of the plugin
    
    
## 0.6.1

Update docs, update editorconfig template


## 0.6.0

1. Improve test coverage, each formatting
options have specific tests
2. Refactor the indentation algorithm to better support the use of tabs as indentation
3. Provides many formatting options related to indentation
4. Some formatting options have been renamed
5. Fixed many bugs


## 0.5.1

Fix the unavailable problem under linux

## 0.5.0

The language service is separated from the io thread and the logic thread by a single thread. The io thread will read the message as fast as possible, and the logical thread is lock-free and asynchronous within a single thread (excluding locks controlled by asio itself)
Incremental update is adopted, and the amount of a single contract is greatly reduced, and the language service adopts an in-place incremental update algorithm (rarely re-applying for space to store codes)
The language service implements the scheduling mechanism, and the code diagnosis implements the back-end anti-shake, and the anti-shake interval is tentatively set at 0.3 seconds.
The performance of the plug-in is greatly improved, code diagnosis is no longer a performance bottleneck, and the code within 100,000 lines will not affect the normal use of the plug-in

## 0.4.5

Fix the problem that the compiler version under linux is gcc11, which makes it unavailable in most linux environments

## 0.4.4

Fix enum inconsistencies
## 0.4.3

fix diagnostic bug

## 0.4.2

1. Modify the formatting rules. If there is an expression form with a single parameter omitting parentheses in the continuous call expression, the parameter will keep a distance from the next call parameter list.
2. Fix a bug of recognition error

## 0.4.1

Provide a special export.rule rule for the module, the usage form is to be considered

## 0.4.0

Provide auto import function

## 0.3.2

Fix the bug that the label causes the formatting result to be unstable

## 0.3.1

Modifying the configuration now refreshes all open files immediately

## 0.3.0

Fix many bugs, refactor the iterative algorithm, and increase test cases

## 0.2.6

Reduce the size of the plug-in, and the plug-in realizes the distribution of different packages by system.

## 0.2.5

Fixed an issue with formatting errors when calling functions continuously

## 0.2.4

Fix the problem that cin.readsome cannot work correctly on linux and macosx, and cin and cout are still used on windows
Use asio encapsulated interface on linux and macosx

## 0.2.2

Optimize the memory usage of nodes, optimize resident memory, use mimalloc to optimize memory allocation performance, and optimize memory fragmentation.

## 0.2.1

Fix memory issues and reduce memory consumption

## 0.2.0

1. Rewrite .editorconfig reading rules, support editorconfig's basic matching rules and all basic options
2. Support lua naming style detection rules based on lua syntax features and daily habits
3. Unnecessary parsing in language service implementation using abnormal early interruption

## 0.1.18
    
Fixed a bug where naming style checking was not working correctly

## 0.1.17

1. Modify the name of the configuration option, which is consistent with the name used by editorconfig (it is recommended to re-import the configuration)

2. Support detecting whether the text ends with a new line

3. Support naming style detection (not enabled by default)

## 0.1.16

Code Diagnostics Localization

## 0.1.15

fix bugs
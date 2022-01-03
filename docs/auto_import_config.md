# Auto Import

现在大部分语言插件都有auto import方面的功能，lua在这方面由于模块化方式不统一，个人对模块的理解和使用上差异较大，导致auto import 方面的插件几乎没有。

该项目提供一种被普遍使用的近乎于标准的引入模块的方式的auto import功能。

## lua.module.json 配置文件

auto import 功能不再使用.editorconfig文件作为配置文件。采用lua.module.json作为配置文件，该文件位于哪个目录内则该目录内的所有lua文件会以该目录内的lua.module.json为模块配置。并且扩展的起始目录为该目录。

如果工作区内存在多个lua.module.json则采用就近原则，也就是说一个文件的模块配置以最靠近它的lua.module.json为准。


### lua.module.json 基本形式
```json
{
    "name": "root",
    "import": [
        "root"
    ]
}
```

该基本形式定义了模块名称和引入模块名，也就是说在模块(lua.modudle.json称为模块)定义了自身名称之后，可以通过import字段引入该名称，对于该模块能够影响的所有lua文件，只能看见到import字段引入的模块名称引入的lua模块路径。

换句话说对于以下目录结构

```plaintext
scirpt
|-lua.module.json 
|-aaa.lua
|-lib
| |-lua.module.json
| |-bbb.lua
```

如果lua.module.json的内容分别
```json
// script/lua.module.json
{
    "name": "root",
    "import": [
        "root", "lib"
    ]
}
// script/lib/lua.module.json
{
    "name": "lib",
    "import": [
        "lib"
    ]
}
```

则lib目录下的bbb文件在auto import时是无法看见aaa.lua文件的因为他没有引入root模块
而script/aaa.lua文件是可以看见bbb.lua文件的，因为他引入了lib模块名

### 配置项 separator

该配置项决定的是auto import时路径的分隔符号，一般来说lua推荐的是 '.' 作为模块路径分隔符号，但是很多项目并不是足够的规范采用 '/' 作为分隔符。

该配置项的默认值是 '.'

### 配置项 import.function

该配置项指示auto import时采用的引入函数，一般来说lua标准是使用 require 函数，但是有很多项目另外造了个用法近似语义略有区别的函数引入模块比如 import

该配置项默认值是 "require"

### 配置项 special.module

该配置项是数组，基本形式是
```json
{
    "special.module": [
        {"name": "socket_core", "module": "socket.core"}
    ]
}
```
该配置项是对某些已有的模块的auto import 匹配名称做设定，当有如上设定时，输入socket_core 时会自动引入socket.core模块


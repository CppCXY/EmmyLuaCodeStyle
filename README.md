# EmmyLuaCodeStyle

## 项目介绍

该项目是基于C++的lua代码格式化算法库

经过长期实践，发现人们对格式化算法的预期是尽可能少的改动代码的行布局，而列布局符合基本审美就可以了。

基于这样的想法我设计并实现了lua格式化算法

## 格式化行为介绍

### 基本语句

该算法的主要特点是分析并对当前代码做出合理的优化,最常见的比如:

```lua
local t=123 --local
print("hello world",
"iiiiii") --call

function fff(aaa,bbb,
    eeee,fff,dddd)

end

```

会被格式化为：
```lua
local t = 123 --local
print("hello",
    "iiiiii") --call

function fff(aaa,bbb,
             eeee,fff,dddd)

end    
```

### 连续赋值语句

对于连续的赋值或者local语句:
```lua
local ttt  = 123 --first
cd        = 345 --second
```
当算法识别到你试图对齐到等号的时候，会采取对齐到等号的方式排版:
```lua
local ttt  = 123 --first
cd         = 345 --second
```

识别的基本原则是对连续的赋值/local/注释语句，当首个赋值或者local语句的等号与左边的元素距离大于1个空格时，该连续的赋值/local语句会对齐到等号。

这里对连续的定义是连续的语句相距不超过2行。

### 表的排版

对常见的表表达式:
```lua
local t = {1,2,3}
local c = {
    aaa, bbb, eee
}
local d = {
    aa  =123,
    bbbb = 4353,
    eee  = 131231,
}
```
我们对他最普遍的预期是:
```lua
local t = { 1, 2, 3 }
local c = {
    aaa, bbb, eee
}
local d = {
    aa   = 123,
    bbbb = 4353,
    eee  = 131231,
}
```
该算法的格式化结果就是上述代码块，其中对 d 表的排版并非严格对齐到等号，他采取和连续赋值/local语句相似的规则，但是表的情况要复杂一些，考察如下语句：
```lua
local d = {
    aa = bbbb, aaa,
    ccc = dddd, --ffff
}
```
该语句也是合法，但是格式化不会采取对齐到等号的算法。





## 格式化选项

待续
## License

没有License

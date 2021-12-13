# EmmyLuaCodeStyle

[English doc](README_EN.md)

## 项目介绍

该项目是基于C++的lua代码解析\格式化\代码诊断算法库和语言服务实例

该项目主要追求的是合理的格式化，多样的代码风格诊断

该项目除了提供语言服务实例，也提供独立的命令行工具CodeFormat，他能用于批量的代码格式化和代码风格检查（还存在很多问题，可以自行修改）。

## 文档

* [格式化行为介绍](docs/format_action.md)
* [如何配置格式化](docs/format_config.md)
* [代码诊断配置](docs/diagnosis_config.md)

# Contribute

任何pr或者issue都是欢迎的

## Build

如果希望能自己编译该项目，确保自己的编译器能基本支持C++20：
* VS2019 16.10以上
* gcc 10以上
* clang 10以上

```
mkdir build && cd build
cmake ..
cmake --build . 

```

## License

没有License

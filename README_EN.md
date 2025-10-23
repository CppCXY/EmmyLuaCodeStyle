# EmmyLuaCodeStyle

_Fast, configurable, and feature-rich Lua formatting and checking language server._

## Introduction

**EmmyLuaCodeStyle** is a flexible and configurable source code checker, formatter and language server for the [Lua programming language](https://wikipedia.org/wiki/Lua). It supports simple extended syntax and offers a variety of formatting styles. Fast even for large codebases, it ensures a consistent, readable, and standardised Lua custom code style across projects.

EmmyLuaCodeStyle supports Lua 5.1, Lua 5.2, Lua 5.3, Lua 5.4, and LuaJIT.

The language server is based on the Language Server Protocol (LSP) and compatible with IDEs that support LSP. This includes Visual Studio Code, IntelliJ IDEA, Neovim and other popular IDEs.

The project also provides an standalone command line interface called CodeFormat, which can be used for batch code checking and formatting.

> The aim of this project is not to create a coding convention for Lua.

## 🧠 Editor integration

- [Lua for VSCode](https://marketplace.visualstudio.com/items?itemName=sumneko.lua) or [EmmyLuaCodeStyle for VSCode](https://marketplace.visualstudio.com/items?itemName=CppCXY.emmylua-codestyle)
- [SumnekoLua for IntelliJ](https://plugins.jetbrains.com/plugin/22315-sumnekolua) or [EmmyLuaCodeStyle in JetBrains IntelliJ](https://plugins.jetbrains.com/plugin/21973-emmyluacodestyle)
- [LuaLS for Neovim](https://github.com/LuaLS/lua-language-server)

Plugins are available for popular editors to provide on-save formatting and real-time code style enforcement.

## ✨ Features

- **80+ configuration options**: Customise indentation, alignment, spacing, and line wrapping using `.editconfig` syntax.
- **Extended syntax support**: Handles EmmyLua annotations and Lua extensions seamlessly.
- **Multiple formatting styles**: Choose from built-in styles or define your own.
- **Robust Lua parser**: Accurately interprets code structure before applying formatting rules, suitable for large projects and batch formatting.
- **Multiple interfaces:** Language Server Protocol, command line interface, Lua or C++-based parser libraries
- Also includes spell-checking and document formatting

## 🚀 Use cases

- Enforcing a unified code style across repositories.
- Automatically formatting configuration and script files.
- Maintaining consistency in multi-contributor Lua codebases.
- Supporting solo developers with specific personal style preferences.

## 📚 Documentation

* [Introduction to formatting behavior](docs/format_action_EN.md)
* [How to configure formatting](docs/format_config_EN.md)
* [How to configure naming style checking](docs/name_style_EN.md)

## 🧑‍💻 Contribute

Pull requests and issue reporting is appreciated. If possible, you can help improve the English documentation.

## 🔨 Build

If you want to compile the project yourself, make sure that your compiler can support [C++20](https://en.cppreference.com/w/cpp/20.html):

* VS2019 16.10 and above
* gcc 10 and above
* clang 10 and above

In the project's root directory, run the following shell commands:

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Contributors

[**@CppCXY**](https://github.com/CppCXY)

**Other contributors**

[**@obszczymucha**](https://github.com/obszczymucha)

[**@Rainer Poisel**](https://github.com/rpoisel)

[**@AndreaWalchshoferSCCH**](https://github.com/AndreaWalchshoferSCCH)

[**@一醉青衫**](https://github.com/qq792453582)

## 📄 License

Licensed under the [MIT License](LICENSE).

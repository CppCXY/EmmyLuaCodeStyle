# EmmyLuaCodeStyle

If possible, you can help me improve the English document 

## Editor Support

vscode:
[vscode-EmmyLuaCodeStyle](https://marketplace.visualstudio.com/items?itemName=CppCXY.emmylua-codestyle)
[vscode-Lua](https://marketplace.visualstudio.com/items?itemName=sumneko.lua)

intellij:
[intellij-EmmyLuaCodeStyle](https://plugins.jetbrains.com/plugin/21973-emmyluacodestyle)

neovim:
[Lua Language Server](https://github.com/LuaLS/lua-language-server)

## Project Introduction 

The project `is not a specification for the Lua code format`, this project is an example of Lua code analysis\formatting\code diagnosis algorithm library and language service based on C++

The main pursuit of this project is reasonable formatting and diagnosis of various code styles

In addition to providing language service examples, the project also provides an independent command line tool CodeFormat, which can be used for batch code formatting and code style checking.

## Feature

* Document formatting
* Range formatting
* Code format check
* Code spell check
* Can be used as a lua library/cli tool/C++ parsing lua library/language server

## RoadMap
* plugin[0%]

## Document

* [Introduction to formatting behavior](docs/format_action_EN.md)
* [How to configure formatting](docs/format_config_EN.md)
* [How to configure naming style checking](docs/name_style_EN.md)

## Contribute
Any pr or issue are welcome 

## Build

If you want to compile the project yourself, make sure that your compiler can basically support C++20: 
* VS2019 16.10 and above
* gcc 10 and above
* clang 10 and above

```
mkdir build && cd build
cmake ..
cmake --build . 

```

## Developed By

[**@CppCXY**](https://github.com/CppCXY)

**Contributors**

[**@obszczymucha**](https://github.com/obszczymucha)

[**@Rainer Poisel**](https://github.com/rpoisel)

[**@AndreaWalchshoferSCCH**](https://github.com/AndreaWalchshoferSCCH)

[**@一醉青衫**](https://github.com/qq792453582)
## License

[MIT](LICENSE)

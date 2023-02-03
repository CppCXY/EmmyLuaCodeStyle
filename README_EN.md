# EmmyLuaCodeStyle

If possible, you can help me improve the English document 

## Project Introduction 

This project is an example of Lua code analysis\formatting\code diagnosis algorithm library and language service based on C++

The main pursuit of this project is reasonable formatting and diagnosis of various code styles

In addition to providing language service examples, the project also provides an independent command line tool CodeFormat, which can be used for batch code formatting and code style checking.

## Feature

* Document formatting
* Range formatting
* Code format check
* Code spell check
* Can be used as a lua library/cli tool/C++ parsing lua library/language server

## RoadMap
* reimplement range formatting[0%]
* reimplement type formatting[0%]
* plugin[0%]

## Document
* [Usage](docs/usage_EN.md)
* [Introduction to formatting behavior](docs/old_version_doc/format_action_EN.md)
* [How to configure formatting](docs/old_version_doc/format_config_EN.md)
* [Code diagnosis configuration](docs/old_version_doc/diagnosis_config_EN.md)
* [Disable format](docs/old_version_doc/disable_format_EN.md)
* [auto import configuration](docs/old_version_doc/auto_import_config_EN.md)
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


## License

[MIT](LICENSE)

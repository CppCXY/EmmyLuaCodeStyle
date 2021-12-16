# EmmyLuaCodeStyle

If possible, you can help me improve the English document 

## Project Introduction 

This project is an example of Lua code analysis\formatting\code diagnosis algorithm library and language service based on C++

The main pursuit of this project is reasonable formatting and diagnosis of various code styles

In addition to providing language service examples, the project also provides an independent command line tool CodeFormat, which can be used for batch code formatting and code style checking (there are still many problems, you can modify it yourself). 

## 文档

* [Introduction to formatting behavior](docs/format_action_EN.md)
* [How to configure formatting](docs/format_config_EN.md)
* [Code diagnosis configuration](docs/diagnosis_config_EN.md)

# Contribute

Any pr or issue are welcome 

## Build

If you want to compile the project yourself, make sure that your compiler can basically support C++20: 
* VS2019 16.10以上
* gcc 10以上
* clang 10以上

```
mkdir build && cd build
cmake ..
cmake --build . 

```

## Developed By

[**@CppCXY**](https://github.com/CppCXY)

**Contributors**


## License

no License

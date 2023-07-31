cmake_minimum_required(VERSION 3.14)

project(LuaParser)

add_library(LuaParser STATIC)

add_dependencies(LuaParser Util)

target_include_directories(LuaParser PUBLIC
        include
        PRIVATE
        src
        )

target_sources(LuaParser
        #src
        PRIVATE
        # parse
        src/Parse/LuaParser.cpp
        src/Parse/Mark.cpp

        # lexer
        src/Lexer/LuaLexer.cpp
        src/Lexer/LuaIdentify.cpp
        src/Lexer/TextReader.cpp

        # ast
        src/Ast/LuaSyntaxNode.cpp
        src/Ast/LuaSyntaxTree.cpp
        src/Ast/LuaSyntaxMultiKind.cpp
        src/Ast/LuaSyntaxNodeKind.cpp
        # types
        src/Types/TextRange.cpp

        # file
        src/File/LuaSource.cpp

        #exception
        src/exception/LuaParseException.cpp
        )

target_link_libraries(LuaParser PUBLIC Util)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(LuaParser PUBLIC /utf-8)
endif ()
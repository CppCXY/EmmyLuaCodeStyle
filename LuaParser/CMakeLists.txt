cmake_minimum_required(VERSION 3.11)

project(LuaParser)

add_library(LuaParser STATIC)

add_dependencies(LuaParser Util)

target_include_directories(LuaParser PUBLIC
        ${LuaCodeStyle_SOURCE_DIR}/include
        src
        )

target_sources(LuaParser
        #src
        PRIVATE
        # parse
        ${LuaParser_SOURCE_DIR}/src/Parse/LuaParser.cpp
        ${LuaParser_SOURCE_DIR}/src/Parse/Mark.cpp

        # lexer
        ${LuaParser_SOURCE_DIR}/src/Lexer/LuaLexer.cpp
        ${LuaParser_SOURCE_DIR}/src/Lexer/LuaIdentify.cpp
        ${LuaParser_SOURCE_DIR}/src/Lexer/TextReader.cpp

        # ast
        ${LuaParser_SOURCE_DIR}/src/Ast/LuaSyntaxNode.cpp
        ${LuaParser_SOURCE_DIR}/src/Ast/LuaSyntaxTree.cpp
        ${LuaParser_SOURCE_DIR}/src/Ast/LuaSyntaxMultiKind.cpp
        ${LuaParser_SOURCE_DIR}/src/Ast/LuaSyntaxNodeKind.cpp
        # types
        ${LuaParser_SOURCE_DIR}/src/Types/TextRange.cpp

        # file
        ${LuaParser_SOURCE_DIR}/src/File/LuaFile.cpp

        #exception
        ${LuaParser_SOURCE_DIR}/src/exception/LuaParseException.cpp
        )

target_link_libraries(LuaParser Util)

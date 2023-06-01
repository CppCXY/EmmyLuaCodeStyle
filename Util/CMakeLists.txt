cmake_minimum_required(VERSION 3.14)

project(Util)

add_subdirectory(${LuaCodeStyle_SOURCE_DIR}/3rd/uriparser uriparser.out)

add_library(Util STATIC)

target_include_directories(Util PUBLIC
        include
        PRIVATE
        src
        ${LuaCodeStyle_SOURCE_DIR}/3rd/uriparser/include
        ${LuaCodeStyle_SOURCE_DIR}/3rd/wildcards/include
        )

target_sources(Util
        PRIVATE
        src/CommandLine.cpp
        src/StringUtil.cpp
        src/Utf8.cpp
        src/Url.cpp
        src/FileFinder.cpp
        src/SymSpell/SymSpell.cpp
        src/SymSpell/SuggestItem.cpp
        src/SymSpell/EditDistance.cpp
        src/InfoTree/InfoTree.cpp
        src/InfoTree/InfoNode.cpp
        )


target_link_libraries(Util PUBLIC uriparser)
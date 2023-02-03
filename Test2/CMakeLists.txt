cmake_minimum_required(VERSION 3.11)

project(FormatTest2)

add_executable(FormatTest2)

add_dependencies(FormatTest2 CodeService LuaParser  Util)

target_include_directories(FormatTest2 PUBLIC
        ${LuaCodeStyle_SOURCE_DIR}/include
        src
        )

target_sources(FormatTest2
        PRIVATE
        src/FormatTest2.cpp
        )

target_link_libraries(FormatTest2 CodeService LuaParser Util)

cmake_minimum_required(VERSION 3.14)

project(LuaCompile)

add_library(LuaCompile STATIC)

target_include_directories(LuaCompile
        PUBLIC
        include
        )


add_subdirectory(src/Lib)
add_subdirectory(src/Lua)
add_subdirectory(src/Workspace)

target_link_libraries(
        LuaCompile
        PUBLIC
        Util
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(LuaCompile PRIVATE /W3 /WX)
    target_compile_options(LuaCompile PRIVATE /utf-8)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(Core PRIVATE -Wall -Werror)
endif ()
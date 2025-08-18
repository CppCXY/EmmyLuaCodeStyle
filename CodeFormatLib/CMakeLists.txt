cmake_minimum_required(VERSION 3.14)

project(CodeFormatLib)

add_library(CodeFormatLib SHARED
        src/LuaJIT-Compact.h
        src/LuaJIT-Compact.cpp)


add_subdirectory(${LuaCodeStyle_SOURCE_DIR}/3rd/lua-5.4.3 lua.out)

set_target_properties(CodeFormatLib PROPERTIES OUTPUT_NAME code_format)

add_dependencies(CodeFormatLib CodeFormatCore lua54)

target_include_directories(CodeFormatLib PUBLIC
        ${LuaCodeStyle_SOURCE_DIR}/3rd/lua-5.4.3/src
        src
)

target_sources(CodeFormatLib
        PRIVATE
        src/CodeFormatLib.cpp
        src/LuaCodeFormat.cpp
        src/LuaJIT-Compact.cpp
)
set_target_properties(CodeFormatLib PROPERTIES PREFIX "")

if (NOT WIN32)
    target_compile_options(CodeFormatLib PUBLIC -fPIC)
endif ()

target_link_libraries(CodeFormatLib PUBLIC CodeFormatCore lua54)

install(TARGETS CodeFormatLib
    LIBRARY DESTINATION lib/lua/5.4
)

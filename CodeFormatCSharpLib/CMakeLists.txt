cmake_minimum_required(VERSION 3.14)

project(CodeFormatCSharpLib)

add_library(CodeFormatCSharpLib SHARED)

set_target_properties(CodeFormatCSharpLib PROPERTIES OUTPUT_NAME code_format_csharp)

add_dependencies(CodeFormatCSharpLib CodeFormatCore)

target_sources(CodeFormatCSharpLib
        PRIVATE
        src/CodeFormatCSharpLib.cpp
        src/CodeFormat.cpp
)

if (NOT WIN32)
    target_compile_options(CodeFormatCSharpLib PUBLIC -fPIC)
endif ()

target_link_libraries(CodeFormatCSharpLib PUBLIC CodeFormatCore)

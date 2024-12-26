cmake_minimum_required(VERSION 3.14)

project(CodeFormatCLib)

add_library(CodeFormatCLib SHARED)

set_target_properties(CodeFormatCLib PROPERTIES OUTPUT_NAME code_format_c)

add_dependencies(CodeFormatCLib CodeFormatCore)

target_include_directories(CodeFormatCLib
		PRIVATE
		include
)

target_sources(CodeFormatCLib
        PRIVATE
        src/CodeFormatCLib.cpp
        src/CodeFormat.cpp
)

target_link_libraries(CodeFormatCLib PUBLIC CodeFormatCore)

install(TARGETS CodeFormatCLib
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
)
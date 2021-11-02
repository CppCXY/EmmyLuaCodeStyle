cmake_minimum_required(VERSION 3.11)

project(CodeFormat)

add_executable(CodeFormat)

add_dependencies(CodeFormat CodeService)

target_include_directories(CodeFormat PUBLIC
	${LuaCodeStyle_SOURCE_DIR}/include
	src
)

target_sources(CodeFormat
	PUBLIC
	${CodeFormat_SOURCE_DIR}/src/CodeFormat.cpp
)

target_link_libraries(CodeFormat CodeService)
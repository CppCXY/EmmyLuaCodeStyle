cmake_minimum_required(VERSION 3.11)



enable_testing()

project(CodeFormatTest)

add_executable(CodeFormatTest)

add_dependencies(CodeFormatTest CodeService Util)

target_include_directories(CodeFormatTest PUBLIC
	${LuaCodeStyle_SOURCE_DIR}/include
	src
	CheckFunctionExists
	CheckCXXSourceCompiles
	CheckLibraryExists
	CPack
)

target_sources(CodeFormatTest
	PUBLIC
	${CodeFormatTest_SOURCE_DIR}/src/CodeFormatTest.cpp
)

target_link_libraries(CodeFormatTest CodeService Util)

add_test(NAME CodeFormatTest COMMAND CodeFormatTest)
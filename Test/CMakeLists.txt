cmake_minimum_required(VERSION 3.11)

project(CodeFormatTest)

add_executable(CodeFormatTest)

add_dependencies(CodeFormatTest CodeService Util)

target_include_directories(CodeFormatTest PUBLIC
	${LuaCodeStyle_SOURCE_DIR}/include
	src
)

target_sources(CodeFormatTest
	PUBLIC
	${CodeFormatTest_SOURCE_DIR}/src/CodeFormatTest.cpp
)

target_link_libraries(CodeFormatTest CodeService Util)

add_test(NAME GrammarTest COMMAND CodeFormatTest CheckGrammar -w ${CodeFormatTest_SOURCE_DIR}/test_script/grammar)
add_test(NAME FormatTest COMMAND CodeFormatTest CheckFormatResult -w ${CodeFormatTest_SOURCE_DIR}/test_script/format_text/wait_format -f ${CodeFormatTest_SOURCE_DIR}/test_script/format_text/wait_format_should_be) 
add_test(NAME FormatByOptionTest COMMAND CodeFormatTest CheckFormatResultByOption -w ${CodeFormatTest_SOURCE_DIR}/test_script/format_text/wait_format_by_option -f ${CodeFormatTest_SOURCE_DIR}/test_script/format_text/wait_format_by_option_should_be) 

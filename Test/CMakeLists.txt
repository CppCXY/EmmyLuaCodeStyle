cmake_minimum_required(VERSION 3.11)

project(CodeFormatTest)

add_executable(CodeFormatTest)

add_dependencies(CodeFormatTest CodeService Util)

target_include_directories(CodeFormatTest PUBLIC
        ${LuaCodeStyle_SOURCE_DIR}/include
        ${LuaCodeStyle_SOURCE_DIR}/3rd/googletest-1.12.1/googletest/include
        src
        )

target_sources(CodeFormatTest
        PRIVATE
        src/main.cpp
        src/TestHelper.cpp
        src/Grammar_unitest.cpp
        src/FormatResult_unitest.cpp
        )

target_link_libraries(CodeFormatTest CodeService Util gtest)
if(WIN32)
    # see https://github.com/google/googletest/issues/4067
    set_property(TARGET gtest PROPERTY MSVC_RUNTIME_LIBRARY "/MD")
    set_property(TARGET gtest_main PROPERTY MSVC_RUNTIME_LIBRARY "/MD")
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

add_test(NAME TEST COMMAND CodeFormatTest ${CodeFormatTest_SOURCE_DIR}/test_script/)

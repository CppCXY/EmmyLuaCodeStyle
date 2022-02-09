cmake_minimum_required(VERSION 3.11)

project(LuaCodeStyle)

set(CMAKE_CXX_STANDARD 20)

option(BuildAsLuaLib "Build for lua dll" OFF)
option(BuildCodeFormat "Build CodeFormat" ON)
option(BuildCodeFormatServer "Build CodeFormatServer" ON)
option(EnableTest "Test project" ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_definitions(-DMSVC)
	# 优化选项
else()
	set(CMAKE_C_FLAGS_RELEASE "-O2")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2")
endif ()

add_subdirectory(LuaParser)
add_subdirectory(CodeService)
add_subdirectory(Util)

if(BuildAsLuaLib)
	add_subdirectory(CodeFormatLib)
endif()

if(BuildCodeFormat)
	add_subdirectory(CodeFormat)
endif()

if(BuildCodeFormatServer)
	add_subdirectory(CodeFormatServer)
endif()

if(EnableTest)
	enable_testing()
	add_subdirectory(Test)
endif()

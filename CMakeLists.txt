cmake_minimum_required(VERSION 3.11)

project(LuaCodeStyle)

set(CMAKE_CXX_STANDARD 20)

option(BuildARM "CROSS COMPILER" OFF)
option(BuildAsLuaLib "Build for lua dll" OFF)
option(BuildCodeFormat "Build CodeFormat" ON)
option(BuildCodeFormatServer "Build CodeFormatServer" ON)
option(EnableTest "Test project" ON)

if(BuildARM)
	SET(CMAKE_C_COMPILER "arm-linux-gcc")
	SET(CMAKE_CXX_COMPILER "arm-linux-g++")
	SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
	SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
	SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_definitions(-DMSVC)
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

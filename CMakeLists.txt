cmake_minimum_required(VERSION 3.11)

project(LuaCodeStyle)

set(CMAKE_CXX_STANDARD 20)

option(BuildAsLuaLib "Build for lua dll" OFF)
option(BuildCodeFormat "Build CodeFormat" ON)
option(BuildCodeFormatServer "Build CodeFormatServer" OFF)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_definitions(-DMSVC)
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
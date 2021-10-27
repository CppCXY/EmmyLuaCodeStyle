cmake_minimum_required(VERSION 3.11)

project(LuaCodeStyle)

set(CMAKE_CXX_STANDARD 20)

# ±‡“Î∆˜
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_definitions(-DMSVC)
endif ()


add_subdirectory(LuaParser)
add_subdirectory(CodeService)
add_subdirectory(3rd/lua-5.4.3)
add_subdirectory(CodeFormatLib)
add_subdirectory(CodeFormat)


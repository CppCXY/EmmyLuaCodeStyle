cmake_minimum_required(VERSION 3.11)

project(LuaCodeStyle)

set(CMAKE_CXX_STANDARD 20)

# ±‡“Î∆˜
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_definitions(-DMSVC)

	set(CMAKE_C_FLAGS_DEBUG "/Od")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "/O2")
	set(CMAKE_C_FLAGS_RELEASE "/O2")
	set(CMAKE_CXX_FLAGS_DEBUG "/Od")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/O2")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2")

else()

    set(CMAKE_C_FLAGS_DEBUG "-O0")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0")
endif ()


add_subdirectory(LuaParser)
add_subdirectory(CodeService)


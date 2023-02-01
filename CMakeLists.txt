cmake_minimum_required(VERSION 3.11)

project(LuaCodeStyle)

set(CMAKE_CXX_STANDARD 20)

option(BuildAsLuaLib "Build for lua dll" OFF)
option(BuildCodeFormat "Build CodeFormat" ON)
option(BuildCodeFormatServer "Build CodeFormatServer" ON)
option(EnableTest "Test project" ON)

if(APPLE)
	include(CheckCCompilerFlag)

	set(CMAKE_REQUIRED_LINK_OPTIONS "-arch;x86_64")
	check_c_compiler_flag("-arch x86_64" x86_64Supported)
	message("x86_64Supported=${x86_64Supported}")

	set(CMAKE_REQUIRED_LINK_OPTIONS "-arch;arm64")
	check_c_compiler_flag("-arch arm64" arm64Supported)
	message("arm64Supported=${arm64Supported}")
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
	add_subdirectory(3rd/googletest-1.12.1)
	add_subdirectory(Test)
	# for feature develop simple test
	add_subdirectory(Test2)
endif()

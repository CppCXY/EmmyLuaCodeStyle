cmake_minimum_required(VERSION 3.11)

project(Util)

add_library(Util STATIC)

target_include_directories(Util PUBLIC
	${LuaCodeStyle_SOURCE_DIR}/include
	PRIVATE
	src
)

target_sources(Util
	PUBLIC
	${LuaCodeStyle_SOURCE_DIR}/include/Util/CommandLine.h
	${LuaCodeStyle_SOURCE_DIR}/include/Util/StringUtil.h
	${LuaCodeStyle_SOURCE_DIR}/include/Util/Utf8.h

	PRIVATE
	${Util_SOURCE_DIR}/src/CommandLine.cpp
	${Util_SOURCE_DIR}/src/StringUtil.cpp
	${Util_SOURCE_DIR}/src/Utf8.cpp
)


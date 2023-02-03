cmake_minimum_required(VERSION 3.11)

project(Util)

add_subdirectory(${LuaCodeStyle_SOURCE_DIR}/3rd/uriparser uriparser.out)

add_library(Util STATIC)

target_include_directories(Util PUBLIC
	${LuaCodeStyle_SOURCE_DIR}/include
	PRIVATE
	src
	${LuaCodeStyle_SOURCE_DIR}/3rd/uriparser/include
	${LuaCodeStyle_SOURCE_DIR}/3rd/wildcards/include
)

target_sources(Util
	PUBLIC
	${LuaCodeStyle_SOURCE_DIR}/include/Util/CommandLine.h
	${LuaCodeStyle_SOURCE_DIR}/include/Util/StringUtil.h
	${LuaCodeStyle_SOURCE_DIR}/include/Util/Utf8.h
	${LuaCodeStyle_SOURCE_DIR}/include/Util/Url.h
	${LuaCodeStyle_SOURCE_DIR}/include/Util/SymSpell/SymSpell.h

	PRIVATE
	${Util_SOURCE_DIR}/src/CommandLine.cpp
	${Util_SOURCE_DIR}/src/StringUtil.cpp
	${Util_SOURCE_DIR}/src/Utf8.cpp
	${Util_SOURCE_DIR}/src/Url.cpp
	${Util_SOURCE_DIR}/src/FileFinder.cpp
	${Util_SOURCE_DIR}/src/SymSpell/SymSpell.cpp
	${Util_SOURCE_DIR}/src/SymSpell/SuggestItem.cpp
	${Util_SOURCE_DIR}/src/SymSpell/EditDistance.cpp
)


target_link_libraries(Util PUBLIC uriparser)
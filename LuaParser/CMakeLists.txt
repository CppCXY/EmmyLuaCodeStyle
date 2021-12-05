cmake_minimum_required(VERSION 3.11)

project(LuaParser)

add_library(LuaParser STATIC)

target_include_directories(LuaParser PUBLIC
	${LuaCodeStyle_SOURCE_DIR}/include
	${LuaCodeStyle_SOURCE_DIR}/3rd/utf8/include
	src
)

target_sources(LuaParser PUBLIC
	#include
	${LuaCodeStyle_SOURCE_DIR}/include/LuaParser/LuaToken.h
	${LuaCodeStyle_SOURCE_DIR}/include/LuaParser/LuaTokenType.h
	${LuaCodeStyle_SOURCE_DIR}/include/LuaParser/LuaTokenParser.h
	${LuaCodeStyle_SOURCE_DIR}/include/LuaParser/LuaParser.h

	${LuaCodeStyle_SOURCE_DIR}/include/LuaParser/LuaAstNode.h
	${LuaCodeStyle_SOURCE_DIR}/include/LuaParser/LuaAstNodeType.h
	${LuaCodeStyle_SOURCE_DIR}/include/LuaParser/TextRange.h
	#src
	PRIVATE
	${LuaParser_SOURCE_DIR}/src/LuaParser.cpp	
	${LuaParser_SOURCE_DIR}/src/LuaTokenParser.cpp	
	${LuaParser_SOURCE_DIR}/src/LuaParseException.cpp
	${LuaParser_SOURCE_DIR}/src/LuaAstNode.cpp
	${LuaParser_SOURCE_DIR}/src/TextRange.cpp
	${LuaParser_SOURCE_DIR}/src/LuaAstVisitor.cpp
	
	${LuaParser_SOURCE_DIR}/src/LuaDefine.h
	${LuaParser_SOURCE_DIR}/src/LuaTokenTypeDetail.h
)
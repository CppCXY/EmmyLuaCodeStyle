cmake_minimum_required(VERSION 3.11)

project(CodeService)

add_executable(CodeService)

add_dependencies(CodeService LuaParser)

target_include_directories(CodeService PUBLIC
	${LuaCodeStyle_SOURCE_DIR}/include
	src
)

target_sources(CodeService 
	PUBLIC
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/LuaFormatter.h	
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/FormatElement.h	
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/IndentElement.h	
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/MinLineElement.h	
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/KeepLineElement.h	
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/FormatElementType.h	
	#include
	PRIVATE
	${CodeService_SOURCE_DIR}/src/CodeService.cpp	
	${CodeService_SOURCE_DIR}/src/LuaFormatter.cpp	

	${CodeService_SOURCE_DIR}/src/FormatElement/IndentElement.cpp	
	${CodeService_SOURCE_DIR}/src/FormatElement/KeepLineElement.cpp	
	${CodeService_SOURCE_DIR}/src/FormatElement/MinLineElement.cpp	
)

target_link_libraries(CodeService LuaParser)
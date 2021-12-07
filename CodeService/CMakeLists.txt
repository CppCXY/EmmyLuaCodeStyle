cmake_minimum_required(VERSION 3.11)

project(CodeService)

add_library(CodeService STATIC)

add_dependencies(CodeService LuaParser Util)

target_include_directories(CodeService PUBLIC
	${LuaCodeStyle_SOURCE_DIR}/include
	src
)

target_sources(CodeService 
	PUBLIC
	#include
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/LuaFormatter.h	
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/FormatElement.h	
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/IndentElement.h	
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/MinLineElement.h	
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/KeepLineElement.h	
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/FormatElementType.h	
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/StatementElement.h	
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/KeepBlankElement.h	
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/LineElement.h
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/AlignmentElement.h
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/AlignmentLayoutElement.h
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/ExpressionElement.h
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/FormatContext.h
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/LongExpressionLayoutElement.h
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/AlignToFirstElement.h
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/KeepElement.h
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/SubExpressionElement.h
	${LuaCodeStyle_SOURCE_DIR}/include/CodeService/FormatElement/TextDefineType.h
	PRIVATE
	${CodeService_SOURCE_DIR}/src/LuaFormatter.cpp	

	${CodeService_SOURCE_DIR}/src/FormatElement/IndentElement.cpp	
	${CodeService_SOURCE_DIR}/src/FormatElement/KeepLineElement.cpp	
	${CodeService_SOURCE_DIR}/src/FormatElement/MinLineElement.cpp	
	${CodeService_SOURCE_DIR}/src/FormatElement/StatementElement.cpp	
	${CodeService_SOURCE_DIR}/src/FormatElement/TextElement.cpp	
	${CodeService_SOURCE_DIR}/src/FormatElement/KeepBlankElement.cpp	
	${CodeService_SOURCE_DIR}/src/FormatElement/FormatElement.cpp	
	${CodeService_SOURCE_DIR}/src/FormatElement/LineElement.cpp	
	${CodeService_SOURCE_DIR}/src/FormatElement/AlignmentElement.cpp	
	${CodeService_SOURCE_DIR}/src/FormatElement/ExpressionElement.cpp	
	${CodeService_SOURCE_DIR}/src/FormatElement/FormatContext.cpp	
	${CodeService_SOURCE_DIR}/src/FormatElement/AlignmentLayoutElement.cpp	
	${CodeService_SOURCE_DIR}/src/FormatElement/LongExpressionLayoutElement.cpp	
	${CodeService_SOURCE_DIR}/src/FormatElement/AlignToFirstElement.cpp	
	${CodeService_SOURCE_DIR}/src/FormatElement/KeepElement.cpp	
	${CodeService_SOURCE_DIR}/src/FormatElement/SubExpressionElement.cpp	
	${CodeService_SOURCE_DIR}/src/FormatElement/DiagnosisContext.cpp	
	${CodeService_SOURCE_DIR}/src/FormatElement/RangeFormatContext.cpp	
	${CodeService_SOURCE_DIR}/src/LuaCodeStyleOptions.cpp
	${CodeService_SOURCE_DIR}/src/LuaEditorConfig.cpp
	${CodeService_SOURCE_DIR}/src/LanguageTranslator.cpp
	${CodeService_SOURCE_DIR}/src/NameStyle/NameStyleChecker.cpp
	${CodeService_SOURCE_DIR}/src/NameStyle/NameStyleRuleMatcher.cpp
	${CodeService_SOURCE_DIR}/src/NameStyle/CheckElement.cpp
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_options(CodeService PUBLIC /utf-8)
endif ()

target_link_libraries(CodeService LuaParser Util)
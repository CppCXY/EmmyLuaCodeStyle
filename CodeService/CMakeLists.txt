cmake_minimum_required(VERSION 3.11)

project(CodeService)

add_library(CodeService STATIC)

add_dependencies(CodeService LuaParser Util)

target_include_directories(CodeService PUBLIC
        ${LuaCodeStyle_SOURCE_DIR}/include
        src
        )

target_sources(CodeService
        PRIVATE
        # config
        ${CodeService_SOURCE_DIR}/src/Config/FunctionOption.cpp
        ${CodeService_SOURCE_DIR}/src/Config/LuaStyle.cpp
        ${CodeService_SOURCE_DIR}/src/Config/LuaEditorConfig.cpp
        ${CodeService_SOURCE_DIR}/src/Config/LuaDiagnosticStyle.cpp
        ${CodeService_SOURCE_DIR}/src/Config/LanguageTranslator.cpp
        ${CodeService_SOURCE_DIR}/src/Config/EditorconfigPattern.cpp

        # format
        ${CodeService_SOURCE_DIR}/src/Format/FormatBuilder.cpp
        ${CodeService_SOURCE_DIR}/src/Format/FormatState.cpp
        ${CodeService_SOURCE_DIR}/src/Format/Analyzer/FormatAnalyzer.cpp
        ${CodeService_SOURCE_DIR}/src/Format/Analyzer/SpaceAnalyzer.cpp
        ${CodeService_SOURCE_DIR}/src/Format/Analyzer/IndentationAnalyzer.cpp
        ${CodeService_SOURCE_DIR}/src/Format/Analyzer/LineBreakAnalyzer.cpp
        ${CodeService_SOURCE_DIR}/src/Format/Analyzer/AlignAnalyzer.cpp
        ${CodeService_SOURCE_DIR}/src/Format/Analyzer/TokenAnalyzer.cpp
        ${CodeService_SOURCE_DIR}/src/Format/Analyzer/FormatResolve.cpp

        ${CodeService_SOURCE_DIR}/src/Format/Analyzer/SyntaxNodeHelper.cpp
        # rangeFormat
        ${CodeService_SOURCE_DIR}/src/RangeFormat/RangeFormatBuilder.cpp
        # typeFormat
        ${CodeService_SOURCE_DIR}/src/TypeFormat/LuaTypeFormat.cpp
        ${CodeService_SOURCE_DIR}/src/TypeFormat/LuaTypeFormatOptions.cpp
        # diagnostic
        ${CodeService_SOURCE_DIR}/src/Diagnostic/DiagnosticBuilder.cpp
        # diagnostic/nameStyle
        ${CodeService_SOURCE_DIR}/src/Diagnostic/NameStyle/NameStyleChecker.cpp
        ${CodeService_SOURCE_DIR}/src/Diagnostic/NameStyle/NameStyleRuleMatcher.cpp
        # diagnostic/spell
        ${CodeService_SOURCE_DIR}/src/Diagnostic/Spell/CodeSpellChecker.cpp
        ${CodeService_SOURCE_DIR}/src/Diagnostic/Spell/IdentifyParser.cpp
        ${CodeService_SOURCE_DIR}/src/Diagnostic/Spell/TextParser.cpp
        )

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(CodeService PUBLIC /utf-8)
endif ()

target_link_libraries(CodeService LuaParser Util)
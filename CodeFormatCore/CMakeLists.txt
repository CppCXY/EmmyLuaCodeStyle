cmake_minimum_required(VERSION 3.14)

project(CodeFormatCore)

add_library(CodeFormatCore STATIC)

add_dependencies(CodeFormatCore LuaParser Util)

target_include_directories(CodeFormatCore
        PUBLIC
        include
        PRIVATE
        src
        )

target_sources(CodeFormatCore
        PRIVATE
        # config
        src/Config/FunctionOption.cpp
        src/Config/LuaStyle.cpp
        src/Config/LuaEditorConfig.cpp
        src/Config/LuaDiagnosticStyle.cpp
        src/Config/LanguageTranslator.cpp
        src/Config/EditorconfigPattern.cpp

        # format
        src/Format/FormatBuilder.cpp
        src/Format/FormatState.cpp
        src/Format/Analyzer/FormatAnalyzer.cpp
        src/Format/Analyzer/SpaceAnalyzer.cpp
        src/Format/Analyzer/IndentationAnalyzer.cpp
        src/Format/Analyzer/LineBreakAnalyzer.cpp
        src/Format/Analyzer/FormatDocAnalyze.cpp
        src/Format/Analyzer/AlignAnalyzer.cpp
        src/Format/Analyzer/TokenAnalyzer.cpp
		src/Format/Analyzer/SemicolonAnalyzer.cpp
        src/Format/Analyzer/FormatResolve.cpp
        src/Format/Analyzer/SyntaxNodeHelper.cpp
        # rangeFormat
        src/RangeFormat/RangeFormatBuilder.cpp
        # typeFormat
        src/TypeFormat/LuaTypeFormat.cpp
        src/TypeFormat/LuaTypeFormatOptions.cpp
        # diagnostic
        src/Diagnostic/DiagnosticBuilder.cpp
        # diagnostic/nameStyle
        src/Diagnostic/NameStyle/NameStyleChecker.cpp
        src/Diagnostic/NameStyle/NameStyleRuleMatcher.cpp
        # diagnostic/spell
        src/Diagnostic/Spell/CodeSpellChecker.cpp
        src/Diagnostic/Spell/IdentifyParser.cpp
        src/Diagnostic/Spell/TextParser.cpp
        # diagnostic/codestyle
        src/Diagnostic/CodeStyle/CodeStyleChecker.cpp
		)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(CodeFormatCore PUBLIC /utf-8)
endif ()

target_link_libraries(CodeFormatCore LuaParser Util)
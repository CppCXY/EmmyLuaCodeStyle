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
        ${CodeService_SOURCE_DIR}/src/Config/LuaStyle.cpp
        ${CodeService_SOURCE_DIR}/src/Config/LuaEditorConfig.cpp

        # format
        ${CodeService_SOURCE_DIR}/src/Format/FormatBuilder.cpp
        ${CodeService_SOURCE_DIR}/src/Format/Analyzer/FormatAnalyzer.cpp
        ${CodeService_SOURCE_DIR}/src/Format/Analyzer/SpaceAnalyzer.cpp
        ${CodeService_SOURCE_DIR}/src/Format/Analyzer/IndentationAnalyzer.cpp
        ${CodeService_SOURCE_DIR}/src/Format/Analyzer/LineBreakAnalyzer.cpp
        ${CodeService_SOURCE_DIR}/src/Format/Analyzer/AlignAnalyzer.cpp
        ${CodeService_SOURCE_DIR}/src/Format/Analyzer/TokenAnalyzer.cpp
        ${CodeService_SOURCE_DIR}/src/Format/Analyzer/FormatResolve.cpp

        ${CodeService_SOURCE_DIR}/src/Format/Analyzer/SyntaxNodeHelper.cpp
        # typeFormat
#        ${CodeService_SOURCE_DIR}/src/TypeFormat/LuaTypeFormat.cpp
#        ${CodeService_SOURCE_DIR}/src/TypeFormat/LuaTypeFormatOptions.cpp

#        ${CodeService_SOURCE_DIR}/src/FormatElement/IndentElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/KeepLineElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/MinLineElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/StatementElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/TextElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/OperatorElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/KeyWordElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/SepElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/SpaceElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/FormatElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/LineElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/NoIndentElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/MaxSpaceElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/AlignmentElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/ExpressionElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/StringLiteralElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/FormatContext.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/SerializeContext.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/AlignmentLayoutElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/LongExpressionLayoutElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/AlignToFirstElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/AlignIfLayoutElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/IndentOnLineBreakElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/KeepElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/PlaceholderElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/SubExpressionElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/MaxLineElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/CallArgsListLayoutElement.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/DiagnosisContext.cpp
#        ${CodeService_SOURCE_DIR}/src/FormatElement/RangeFormatContext.cpp
#        ${CodeService_SOURCE_DIR}/src/LuaStyle.cpp
#        ${CodeService_SOURCE_DIR}/src/LuaEditorConfig.cpp
#        ${CodeService_SOURCE_DIR}/src/LanguageTranslator.cpp
#        # nameStyle
#        ${CodeService_SOURCE_DIR}/src/NameStyle/NameStyleChecker.cpp
#        ${CodeService_SOURCE_DIR}/src/NameStyle/NameStyleRuleMatcher.cpp
#        ${CodeService_SOURCE_DIR}/src/NameStyle/CheckElement.cpp
#        # spell
#        ${CodeService_SOURCE_DIR}/src/Spell/CodeSpellChecker.cpp
#        ${CodeService_SOURCE_DIR}/src/Spell/IdentifyParser.cpp
#        ${CodeService_SOURCE_DIR}/src/Spell/TextParser.cpp
#        ${CodeService_SOURCE_DIR}/src/AstUtil.cpp
        )

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(CodeService PUBLIC /utf-8)
endif ()

target_link_libraries(CodeService LuaParser Util)
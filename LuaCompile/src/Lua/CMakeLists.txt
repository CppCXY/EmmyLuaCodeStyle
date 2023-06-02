target_sources(LuaCompile
        # src
        PRIVATE

        # lexer
        LuaLexer/LuaLexer.cpp

        # parse
        LuaParser/LuaParser.cpp

        # docLexer
        DocLexer/LuaDocLexer.cpp
        # docParser
        DocParser/LuaDocParser.cpp

        # util
        Util/TextReader.cpp

        # parseState
        ParseState/Mark.cpp
        ParseState/ParseState.cpp

        # define
        Define/LuaIdentify.cpp
        Define/LuaOperatorType.cpp

        # syntax
        SyntaxNode/LuaSyntaxNode.cpp
        SyntaxNode/LuaSyntaxToken.cpp
        SyntaxNode/LuaSyntaxs.cpp

        # iterator
        SyntaxNode/Iterator/SyntaxNodeIterator.cpp

        # kind
        Kind/LuaSyntaxMultiKind.cpp
        # tree
        SyntaxTree/LuaNodeOrToken.cpp
        SyntaxTree/LuaSyntaxTree.cpp
        SyntaxTree/LuaTreeBuilder.cpp
        SyntaxTree/LuaDocTreeBuilder.cpp
        SyntaxTree/TreeUpdateEvent.cpp

        # source
        Source/LuaSource.cpp
        Source/SourceUpdateEvent.cpp

        # exception
        exception/LuaParseException.cpp

        )

#pragma once

#include <memory>
#include <vector>

#include "LuaCompile/Lua/Define/LuaOperatorType.h"
#include "LuaCompile/Lua/Define/LuaSyntaxError.h"
#include "LuaCompile/Lua/Kind/LuaSyntaxNodeKind.h"
#include "LuaCompile/Lua/Kind/LuaTokenKind.h"
#include "LuaCompile/Lua/ParseState/ParseState.h"

class LuaSource;

class LuaDocParser {
public:
    LuaDocParser(const LuaSource *source, std::vector<LuaToken> &&tokens);

    bool Parse();

    std::vector<LuaToken> &GetTokens();

    ParseState &GetParseState();

    std::vector<LuaSyntaxError> &GetErrors();

private:
    void Next();

    LuaTokenKind LookAhead();

    LuaTokenKind Current();

    void Comments();

    void ParseToLineEnd();

    CompleteMarker Comment();

    CompleteMarker EmmyLuaDoc();

    CompleteMarker DocClass();

    CompleteMarker DocInterface();

    CompleteMarker DocEnum();

    CompleteMarker DocAlias();

    CompleteMarker DocField();

    CompleteMarker DocType();

    CompleteMarker DocParam();

    CompleteMarker DocOverload();

    CompleteMarker DocGeneric();

    CompleteMarker DocSee();

    CompleteMarker DocSince();

    CompleteMarker DocDeprecated();

    CompleteMarker DocPublic();

    CompleteMarker DocPrivate();

    CompleteMarker DocProtected();

    CompleteMarker DocOverride();

    CompleteMarker DocVersion();

    CompleteMarker DocLanguage();

    CompleteMarker DocDiagnostic();

    CompleteMarker NormalComment();

    CompleteMarker EnumField();

    CompleteMarker FieldIndex();

    CompleteMarker GenericDefList();

    CompleteMarker TypeList();

    CompleteMarker Type();

    CompleteMarker PrimaryType();

    CompleteMarker TableType();

    CompleteMarker TableFieldType();

    CompleteMarker IdType();

    CompleteMarker StringType();

    CompleteMarker FunctionType();

    CompleteMarker FunctionParamList();

    CompleteMarker FunctionParam();

    CompleteMarker ParType();

    CompleteMarker GenericParamTypeList();

    CompleteMarker GenericDeclare();

    bool TestAndNext(LuaTokenKind kind);

    const LuaSource *_source;
    ParseState _p;
    std::vector<LuaToken> _tokens;
    std::size_t _tokenIndex;
    bool _invalid;
    LuaTokenKind _current;
    std::vector<LuaSyntaxError> _errors;
};

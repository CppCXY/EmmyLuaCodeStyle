#pragma once

#include <map>
#include <stack>
#include <string_view>
#include <vector>
#include "LuaCompile/Lua/Define/LuaToken.h"
#include "LuaCompile/Lua/Util/TextReader.h"
#include "LuaCompile/Lua/Define/LuaSyntaxError.h"


class LuaDocLexer {
public:
    enum class State {
        Init,
        ReadTag,
        TagClass,
        GenericDeclareList,

        TagField,
        TagAlias,

        TagGeneric,
        TagType,
        Type,
        TagParam,
        TagExpectedId,
        TagDiagnostic,

        Continue,
        ReadComment,
        ReadRest,
    };

    explicit LuaDocLexer(std::string_view source, std::size_t offset);

    std::vector<LuaToken> &Tokenize();

    std::vector<LuaSyntaxError>& GetErrors();
private:
    static std::map<std::string, LuaTokenKind, std::less<>> LuaTag;

    static bool IsWhitespace(int ch);

    static bool IsIdStart(int ch);

    static bool IsIdContinue(int ch);

    LuaTokenKind Lex();

    LuaTokenKind ReadInit();

    LuaTokenKind ReadTag();

    LuaTokenKind ReadTagClass();

    LuaTokenKind ReadGenericDeclareList();

    LuaTokenKind ReadTagField();

    LuaTokenKind ReadTagAlias();

    LuaTokenKind ReadTagType();

    LuaTokenKind ReadTagGeneric();

    LuaTokenKind ReadTagParam();

    LuaTokenKind ReadTagExpectedId();

    LuaTokenKind ReadTagDiagnostic();

    LuaTokenKind ReadType();

    LuaTokenKind ReadCommentString();

    LuaTokenKind ReadNumeral();

    void TokenError(std::string_view message, TextRange range);

    void TokenError(std::string_view message, std::size_t offset);

    LuaTokenKind ReadId();

    LuaTokenKind ReadWhitespace();

    LuaTokenKind ReadRest();

    LuaTokenKind ReadString();

    void ExpectedType();

    void PushState(State state);

    void PopState();

    void ChangeState(State state);

    TextReader _reader;
    std::vector<LuaToken> _tokens;

    std::stack<State> _stateStack;
    std::size_t _typeLevel;
    bool _typeReq;

    std::vector<LuaSyntaxError> _errors;
};

#include "LuaCompile/Lua/DocParser/LuaDocParser.h"
#include "LuaCompile/Lua/Source/LuaSource.h"

using enum LuaTokenKind;

#define CheckTokenAndNext(tk) \
    if (!TestAndNext(tk)) { return m.UnComplete(_p); }

#define TryAndNext(f) \
    if (f.IsNone()) { return m.UnComplete(_p); }

LuaDocParser::LuaDocParser(const LuaSource *source, std::vector<LuaToken> &&tokens)
    : _source(source),
      _tokens(tokens),
      _tokenIndex(0),
      _invalid(true),
      _current(TK_EOF) {
}

bool LuaDocParser::Parse() {
    Comments();

    return true;
}

std::vector<LuaToken> &LuaDocParser::GetTokens() {
    return _tokens;
}

ParseState &LuaDocParser::GetParseState() {
    return _p;
}

std::vector<LuaSyntaxError> &LuaDocParser::GetErrors() {
    return _errors;
}

void LuaDocParser::Next() {
    auto tk = Current();
    auto me = MarkEvent(MarkEventType::EatToken);
    me.U.Token.Kind = tk;
    me.U.Token.Index = _tokenIndex;
    _p.GetEvents().push_back(me);
    _tokenIndex++;
    _invalid = true;
}

LuaTokenKind LuaDocParser::LookAhead() {
    std::size_t nextIndex = _tokenIndex + 1;

    if (nextIndex < _tokens.size()) {
        return _tokens[nextIndex].TokenType;
    }

    return TK_EOF;
}

LuaTokenKind LuaDocParser::Current() {
    if (_invalid) {
        _invalid = false;
        if (_tokenIndex < _tokens.size()) {
            _current = _tokens[_tokenIndex].TokenType;
        } else {
            _current = TK_EOF;
        }
    }

    return _current;
}

void LuaDocParser::Comments() {
    while (Current() != TK_EOF) {
        if (Comment().IsNone()) {
            ParseToLineEnd();
        }
    }
}

void LuaDocParser::ParseToLineEnd() {
    while (true) {
        auto current = Current();
        if (current == TK_EOF || current == TK_DASHES || current == TK_DASH_DASH_DASH) {
            break;
        }
        Next();
    }
}

CompleteMarker LuaDocParser::Comment() {
    if (Current() == TK_DASH_DASH_DASH) {
        Next();
        if (Current() == TK_AT) {
            Next();
            return EmmyLuaDoc();
        }
    }
    return NormalComment();
}

CompleteMarker LuaDocParser::EmmyLuaDoc() {
    switch (Current()) {
        case TK_TAG_CLASS: {
            return DocClass();
        }
        case TK_TAG_INTERFACE: {
            return DocInterface();
        }
        case TK_TAG_ENUM: {
            return DocEnum();
        }
        case TK_TAG_ALIAS: {
            return DocAlias();
        }
        case TK_TAG_TYPE: {
            return DocType();
        }
        case TK_TAG_PARAM: {
            return DocParam();
        }
        case TK_TAG_FIELD: {
            return DocField();
        }
        case TK_TAG_GENERIC: {
            return DocGeneric();
        }
        case TK_TAG_SEE: {
            return DocSee();
        }
        case TK_TAG_SINCE: {
            return DocSince();
        }
        case TK_TAG_OVERLOAD: {
            return DocOverload();
        }
        case TK_TAG_OVERRIDE: {
            return DocOverride();
        }
        case TK_TAG_VERSION: {
            return DocVersion();
        }
        case TK_TAG_DEPRECATED: {
            return DocDeprecated();
        }
        case TK_TAG_PUBLIC: {
            return DocPublic();
        }
        case TK_TAG_PRIVATE: {
            return DocPrivate();
        }
        case TK_TAG_PROTECTED: {
            return DocProtected();
        }
        case TK_TAG_LANGUAGE: {
            return DocLanguage();
        }
        case TK_TAG_DIAGNOSTIC: {
            return DocDiagnostic();
        }
        default: {
            return CompleteMarker();
        }
    }
}

bool LuaDocParser::TestAndNext(LuaTokenKind kind) {
    if (Current() == kind) {
        Next();
        return true;
    }
    return false;
}

CompleteMarker LuaDocParser::DocClass() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::DocClass);

    // class
    CheckTokenAndNext(TK_TAG_CLASS);

    CheckTokenAndNext(TK_NAME);

    if (Current() == TK_LT) {
        TryAndNext(GenericDefList());
    }

    if (Current() == TK_COLON) {
        Next();
        TryAndNext(TypeList())
    }

    TestAndNext(TK_AT);

    TestAndNext(TK_COMMENT_CONTENT);

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::DocInterface() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::DocInterface);

    // interface
    CheckTokenAndNext(TK_TAG_INTERFACE);

    CheckTokenAndNext(TK_NAME);

    if (Current() == TK_LT) {
        TryAndNext(GenericDefList());
    }

    if (Current() == TK_COLON) {
        Next();
        TryAndNext(TypeList())
    }

    TestAndNext(TK_AT);

    TestAndNext(TK_COMMENT_CONTENT);

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::DocEnum() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::DocEnum);

    // enum
    CheckTokenAndNext(TK_TAG_ENUM);

    CheckTokenAndNext(TK_NAME);

    if (Current() == TK_COLON) {
        TryAndNext(Type());
    }

    if (TestAndNext(TK_AT)) {
        TestAndNext(TK_COMMENT_CONTENT);
    }

    do {
        if (Current() == TK_DASH_DASH_DASH && LookAhead() == TK_OR) {
            Next();
            Next();
        } else {
            break;
        }
    } while (EnumField().IsComplete());

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::DocAlias() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::DocAlias);

    // alias
    CheckTokenAndNext(TK_TAG_ALIAS);

    CheckTokenAndNext(TK_NAME);

    while (Type().IsComplete()) {
        if (TestAndNext(TK_AT)) {
            TestAndNext(TK_COMMENT_CONTENT);
        }

        if (Current() == TK_DASH_DASH_DASH && LookAhead() == TK_OR) {
            Next();
            Next();
        } else {
            break;
        }
    }

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::DocField() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::DocField);

    CheckTokenAndNext(TK_TAG_FIELD);

    TestAndNext(TK_ACCESS);

    switch (Current()) {
        case TK_NAME: {
            Next();
            TestAndNext(TK_NULLABLE);
            TryAndNext(Type());
            break;
        }
        case TK_LBRACKET: {
            TryAndNext(FieldIndex());
            TestAndNext(TK_NULLABLE);
            TryAndNext(Type());
            break;
        }
        default: {
            return m.UnComplete(_p);
        }
    }

    TestAndNext(TK_AT);

    TestAndNext(TK_COMMENT_CONTENT);

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::DocType() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::DocType);

    CheckTokenAndNext(TK_TAG_TYPE);

    TryAndNext(TypeList());

    TestAndNext(TK_AT);

    TestAndNext(TK_COMMENT_CONTENT);

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::DocParam() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::DocParam);

    CheckTokenAndNext(TK_TAG_PARAM);

    CheckTokenAndNext(TK_NAME);

    TestAndNext(TK_NULLABLE);

    TryAndNext(Type());

    TestAndNext(TK_AT);

    TestAndNext(TK_COMMENT_CONTENT);

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::DocOverload() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::DocOverload);

    CheckTokenAndNext(TK_TAG_OVERLOAD);

    TryAndNext(FunctionType());

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::DocGeneric() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::DocGeneric);

    CheckTokenAndNext(TK_TAG_GENERIC);

    while (GenericDeclare().IsComplete()) {
        if (!TestAndNext(TK_COMMA)) {
            break;
        }
    }

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::DocSee() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::DocSee);

    CheckTokenAndNext(TK_TAG_SEE);

    CheckTokenAndNext(TK_NAME);

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::DocSince() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::DocSince);

    CheckTokenAndNext(TK_TAG_SINCE);

    CheckTokenAndNext(TK_NAME);

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::DocDeprecated() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::DocDeprecated);

    CheckTokenAndNext(TK_TAG_DEPRECATED);

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::DocPublic() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::DocPublic);

    CheckTokenAndNext(TK_TAG_PUBLIC);

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::DocPrivate() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::DocPrivate);

    CheckTokenAndNext(TK_TAG_PRIVATE);

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::DocProtected() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::DocProtected);

    CheckTokenAndNext(TK_TAG_PROTECTED);

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::DocOverride() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::DocOverride);

    CheckTokenAndNext(TK_TAG_OVERRIDE);

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::DocVersion() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::DocVersion);

    CheckTokenAndNext(TK_TAG_VERSION);

    CheckTokenAndNext(TK_NAME);

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::DocLanguage() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::DocVersion);

    CheckTokenAndNext(TK_TAG_VERSION);

    CheckTokenAndNext(TK_NAME);

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::DocDiagnostic() {
    return CompleteMarker();
}

CompleteMarker LuaDocParser::NormalComment() {
    auto m = _p.Mark();

    if (Current() == TK_DASHES) {
        Next();
    }

    if (Current() == TK_COMMENT_CONTENT) {
        Next();
    }

    if (Current() == TK_REST) {
        Next();
    }

    return m.Complete(_p, LuaSyntaxNodeKind::NormalComment);
}

CompleteMarker LuaDocParser::EnumField() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::EnumField);

    CheckTokenAndNext(TK_NAME);

    if (TestAndNext(TK_EQ)) {
        Next();
        if (Current() == TK_STRING || Current() == TK_NUMBER) {
            Next();
        }
    }

    TestAndNext(TK_AT);

    TestAndNext(TK_COMMENT_CONTENT);

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::FieldIndex() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::FieldIndex);

    CheckTokenAndNext(TK_LBRACKET);

    switch (Current()) {
        case TK_NAME: {
            TryAndNext(Type());
            break;
        }
        case TK_NUMBER:
        case TK_STRING: {
            Next();
            break;
        }
        default: {
            return m.UnComplete(_p);
        }
    }

    CheckTokenAndNext(TK_RBRACKET);

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::GenericDefList() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::GenericDefList);

    CheckTokenAndNext(TK_LT);

    do {
        CheckTokenAndNext(TK_NAME);
    } while (TestAndNext(TK_COMMA));

    CheckTokenAndNext(TK_GT);

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::TypeList() {
    auto m = _p.Mark();

    do {
        if (Type().IsNone()) {
            break;
        }
    } while (TestAndNext(TK_COMMA));

    return m.Complete(_p, LuaSyntaxNodeKind::TypeList);
}

CompleteMarker LuaDocParser::Type() {
    auto pm = PrimaryType();
    auto cm = pm;
    if (pm.IsComplete()) {
        while (true) {
            switch (Current()) {
                case TK_OR: {
                    auto m = cm.Precede(_p);
                    m.PreComplete(_p, LuaSyntaxNodeKind::UnionType);
                    Next();
                    TryAndNext(Type());
                    cm = m.Complete(_p);
                    break;
                }
                case TK_ARR: {
                    auto m = cm.Precede(_p);
                    Next();
                    cm = m.Complete(_p, LuaSyntaxNodeKind::ArrayType);
                    break;
                }
                default: {
                    goto endLoop;
                }
            }
        }
    endLoop:
        void(0);
    }
    return cm;
}

CompleteMarker LuaDocParser::PrimaryType() {
    switch (Current()) {
        case TK_LCURLY: {
            return TableType();
        }
        case TK_NAME: {
            auto cm = IdType();
            if (cm.IsComplete()) {
                if (Current() == TK_LT) {
                    auto m = cm.Precede(_p);
                    m.PreComplete(_p, LuaSyntaxNodeKind::GenericType);

                    GenericParamTypeList();

                    cm = m.Complete(_p);
                }
            }
            return cm;
        }
        case TK_STRING: {
            return StringType();
        }
        case TK_FUN: {
            return FunctionType();
        }
        case TK_LPARN: {
            return ParType();
        }
        default: {
            return CompleteMarker();
        }
    }
}

CompleteMarker LuaDocParser::TableType() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::TableType);

    CheckTokenAndNext(TK_LCURLY);

    while (TableFieldType().IsComplete()) {
        switch (Current()) {
            case TK_COMMA:
            case TK_SEMI: {
                Next();
                break;
            }
            default: {
                goto endLoop;
            }
        }
    }
endLoop:

    CheckTokenAndNext(TK_RCURLY);

    return m.Complete(_p);
}

// { aaaa: number, [1]: number, [string]: number }
CompleteMarker LuaDocParser::TableFieldType() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::TableTypeField);

    if (Current() == TK_NAME) {
        Next();
    } else if (Current() == TK_LBRACKET) {
        Next();
        TryAndNext(Type());
        CheckTokenAndNext(TK_RBRACKET);
    }

    CheckTokenAndNext(TK_COLON);
    TryAndNext(Type());

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::IdType() {
    if (Current() == TK_NAME) {
        auto m = _p.Mark();

        Next();

        return m.Complete(_p, LuaSyntaxNodeKind::IdType);
    }
    return CompleteMarker();
}

CompleteMarker LuaDocParser::StringType() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::StringType);

    CheckTokenAndNext(TK_STRING);

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::FunctionType() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::FunctionType);

    CheckTokenAndNext(TK_FUN);

    TryAndNext(FunctionParamList());

    if (Current() == TK_COLON) {
        Next();
        TryAndNext(Type());
    }

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::FunctionParamList() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::FunctionTypeParamTypeList);

    CheckTokenAndNext(TK_LPARN);

    while (FunctionParam().IsComplete()) {
        if (!TestAndNext(TK_COMMA)) {
            break;
        }
    }

    CheckTokenAndNext(TK_RPARN);

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::FunctionParam() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::FunctionParam);

    CheckTokenAndNext(TK_NAME);

    if (TestAndNext(TK_COLON)) {
        TryAndNext(Type());
    }

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::ParType() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::ParType);

    CheckTokenAndNext(TK_LPARN);

    TryAndNext(Type());

    CheckTokenAndNext(TK_RPARN);

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::GenericParamTypeList() {
    auto m = _p.Mark();

    CheckTokenAndNext(TK_LT);

    while (Type().IsComplete()) {
        if (!TestAndNext(TK_COMMA)) {
            break;
        }
    }

    CheckTokenAndNext(TK_GT);

    return m.Complete(_p);
}

CompleteMarker LuaDocParser::GenericDeclare() {
    auto m = _p.Mark();
    m.PreComplete(_p, LuaSyntaxNodeKind::GenericDeclare);

    CheckTokenAndNext(TK_NAME);

    if (TestAndNext(TK_COLON)) {
        TryAndNext(Type());
    }

    return m.Complete(_p);
}

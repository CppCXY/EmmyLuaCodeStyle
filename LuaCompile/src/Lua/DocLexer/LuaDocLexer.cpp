#include "LuaCompile/Lua/DocLexer/LuaDocLexer.h"
#include "LuaCompile/Lua/Define/LuaDefine.h"
#include "LuaCompile/Lua/Define/LuaIdentify.h"

using enum LuaTokenKind;

std::map<std::string, LuaTokenKind, std::less<>> LuaDocLexer::LuaTag = {
    {"class", TK_TAG_CLASS},
    {"enum", TK_TAG_ENUM},
    {"interface", TK_TAG_INTERFACE},
    {"field", TK_TAG_FIELD},
    {"generic", TK_TAG_GENERIC},
    {"type", TK_TAG_TYPE},
    {"param", TK_TAG_PARAM},
    {"alias", TK_TAG_ALIAS},
    {"private", TK_TAG_PRIVATE},
    {"public", TK_TAG_PUBLIC},
    {"protected", TK_TAG_PROTECTED},
    {"overload", TK_TAG_OVERLOAD},
    {"override", TK_TAG_OVERRIDE},
    {"language", TK_TAG_LANGUAGE},
    {"deprecated", TK_TAG_DEPRECATED},
    {"since", TK_TAG_SINCE},
    {"version", TK_TAG_VERSION},
    {"diagnostic", TK_TAG_DIAGNOSTIC}};

LuaDocLexer::LuaDocLexer(std::string_view source, std::size_t offset)
    : _reader(source, offset),
      _typeLevel(0),
      _typeReq(false) {
}

std::vector<LuaToken> &LuaDocLexer::Tokenize() {
    PushState(State::Init);

    while (true) {
        auto type = Lex();
        if (type == TK_EOF) {
            break;
        }

        if (type != TK_WS && type != TK_RETRY) {
            _tokens.emplace_back(type, _reader.GetTokenRange());
        }
    }

    return _tokens;
}

bool LuaDocLexer::IsWhitespace(int ch) {
    return ch > 0 && std::isspace(ch);
}

bool LuaDocLexer::IsIdStart(int ch) {
    return lislalpha(ch);
}

bool LuaDocLexer::IsIdContinue(int ch) {
    return lislalnum(ch) || ch == '-' || ch == '.';
}

LuaTokenKind LuaDocLexer::Lex() {
    _reader.ResetBuffer();
    if (_reader.IsEof() || _stateStack.empty()) {
        return TK_EOF;
    }

    switch (_stateStack.top()) {
        case State::Init: {
            return ReadInit();
        }
        case State::ReadTag: {
            return ReadTag();
        }
        case State::TagClass: {
            return ReadTagClass();
        }
        case State::GenericDeclareList: {
            return ReadGenericDeclareList();
        }
        case State::TagField: {
            return ReadTagField();
        }
        case State::TagAlias: {
            return ReadTagAlias();
        }
        case State::TagGeneric: {
            return ReadTagGeneric();
        }
        case State::TagType: {
            return ReadTagType();
        }
        case State::Type: {
            return ReadType();
        }
        case State::TagParam: {
            return ReadTagParam();
        }
        case State::TagExpectedId: {
            return ReadTagExpectedId();
        }
        case State::TagDiagnostic: {
            return ReadTagDiagnostic();
        }
        case State::ReadComment: {
            return ReadCommentString();
        }
        case State::ReadRest:
            return ReadRest();
        default: {
            return TK_EOF;
        }
    }
}

LuaTokenKind LuaDocLexer::ReadInit() {
    switch (_reader.GetCurrentChar()) {
        case '-': {
            auto count = _reader.EatWhen('-');
            if (count == 3) {
                return TK_DASH_DASH_DASH;
            } else {
                ChangeState(State::ReadRest);
                return TK_DASHES;
            }
        }
        case '@': {
            _reader.SaveAndNext();
            ChangeState(State::ReadTag);
            return TK_AT;
        }
        case '|': {
            _reader.SaveAndNext();
            ChangeState(State::Continue);
            return TK_OR;
        }
        default: {
            if (IsWhitespace(_reader.GetCurrentChar())) {
                return ReadWhitespace();
            }
            return ReadCommentString();
        }
    }
}

LuaTokenKind LuaDocLexer::ReadTag() {
    if (IsIdStart(_reader.GetCurrentChar())) {
        ReadId();
        auto tagId = _reader.GetSaveText();
        auto it = LuaTag.find(tagId);
        if (it != LuaTag.end()) {
            switch (it->second) {
                case TK_TAG_CLASS:
                case TK_TAG_ENUM:
                case TK_TAG_INTERFACE: {
                    ChangeState(State::TagClass);
                    break;
                }
                case TK_TAG_FIELD: {
                    ChangeState(State::TagField);
                    break;
                }
                case TK_TAG_GENERIC: {
                    ChangeState(State::TagGeneric);
                    break;
                }
                case TK_TAG_OVERLOAD:
                case TK_TAG_TYPE: {
                    ChangeState(State::TagType);
                    break;
                }
                case TK_TAG_PARAM: {
                    ChangeState(State::TagParam);
                    break;
                }
                case TK_TAG_LANGUAGE:
                case TK_TAG_SINCE:
                case TK_TAG_VERSION:
                case TK_TAG_SEE: {
                    ChangeState(State::TagExpectedId);
                    break;
                }
                case TK_TAG_PRIVATE:
                case TK_TAG_PUBLIC:
                case TK_TAG_PROTECTED:
                case TK_TAG_DEPRECATED:
                case TK_TAG_OVERRIDE: {
                    break;
                }
                default: {
                    break;
                }
            }
            return it->second;
        }
    }
    return ReadCommentString();
}

LuaTokenKind LuaDocLexer::ReadTagClass() {
    auto ch = _reader.GetCurrentChar();
    switch (ch) {
        case ':': {
            _reader.SaveAndNext();
            _typeReq = true;
            PushState(State::Type);
            return TK_COLON;
        }
        case '<': {
            PushState(State::GenericDeclareList);
            return TK_RETRY;
        }
        case '@': {
            _reader.SaveAndNext();
            ChangeState(State::ReadComment);
            return TK_AT;
        }
        default: {
            if (IsWhitespace(ch)) {
                return ReadWhitespace();
            } else if (IsIdStart(ch)) {
                return ReadId();
            }

            return ReadRest();
        }
    }
}

LuaTokenKind LuaDocLexer::ReadGenericDeclareList() {
    switch (_reader.GetCurrentChar()) {
        case ',': {
            _reader.SaveAndNext();
            return TK_COMMA;
        }
        case '<': {
            _reader.SaveAndNext();
            _typeLevel++;
            return TK_LT;
        }
        case '>': {
            _reader.SaveAndNext();
            _typeLevel--;
            return TK_GT;
        }
        default: {
            auto ch = _reader.GetCurrentChar();
            if (IsWhitespace(ch)) {
                return ReadWhitespace();
            } else if (_typeLevel > 0 && IsIdStart(ch)) {
                return ReadId();
            }

            PopState();
            return TK_RETRY;
        }
    }
}

LuaTokenKind LuaDocLexer::ReadTagField() {
    auto ch = _reader.GetCurrentChar();
    switch (ch) {
        case '[': {
            _reader.SaveAndNext();
            _typeLevel++;
            return TK_LBRACKET;
        }
        case ']': {
            _reader.SaveAndNext();
            _typeLevel--;
            return TK_RBRACKET;
        }
        case '\'':
        case '\"': {
            return ReadString();
        }
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            return ReadNumeral();
        }
        case '@': {
            _reader.SaveAndNext();
            ChangeState(State::ReadComment);
            return TK_AT;
        }
        case '?': {
            _reader.SaveAndNext();
            ExpectedType();
            return TK_NULLABLE;
        }
        default: {
            if (IsWhitespace(ch)) {
                return ReadWhitespace();
            } else if (IsIdStart(ch)) {
                ReadId();
                auto id = _reader.GetSaveText();
                if (_typeLevel == 0) {
                    if (id == "private" || id == "protected" || id == "public") {
                        return TK_ACCESS;
                    }
                    if (_reader.GetCurrentChar() != '?') {
                        ExpectedType();
                    }
                }

                return TK_NAME;
            }

            return ReadRest();
        }
    }
}

LuaTokenKind LuaDocLexer::ReadTagAlias() {
    auto ch = _reader.GetCurrentChar();
    switch (ch) {
        case '@': {
            _reader.SaveAndNext();
            ChangeState(State::ReadComment);
            return TK_AT;
        }
        default: {
            if (IsIdStart(ch)) {
                ExpectedType();
                return ReadId();
            } else if (IsWhitespace(ch)) {
                return ReadWhitespace();
            } else {
                return ReadRest();
            }
        }
    }
}

LuaTokenKind LuaDocLexer::ReadTagType() {
    auto ch = _reader.GetCurrentChar();
    switch (ch) {
        case '@': {
            _reader.SaveAndNext();
            ChangeState(State::ReadComment);
            return TK_AT;
        }
        default: {
            _typeReq = true;
            ChangeState(State::Type);
            return TK_RETRY;
        }
    }
}

LuaTokenKind LuaDocLexer::ReadTagGeneric() {
    switch (_reader.GetCurrentChar()) {
        case ':': {
            _reader.SaveAndNext();
            ExpectedType();
            return TK_COLON;
        }
        case ',': {
            _reader.SaveAndNext();
            return TK_COMMA;
        }
        default: {
            auto ch = _reader.GetCurrentChar();
            if (IsIdStart(ch)) {
                return ReadId();
            } else if (IsWhitespace(ch)) {
                return ReadWhitespace();
            }

            return ReadString();
        }
    }
}

LuaTokenKind LuaDocLexer::ReadTagParam() {
    switch (_reader.GetCurrentChar()) {
        case '@': {
            _reader.SaveAndNext();
            ChangeState(State::ReadComment);
            return TK_AT;
        }
        case '?': {
            _reader.SaveAndNext();
            ExpectedType();
            return TK_NULLABLE;
        }
        case '.': {
            _reader.SaveAndNext();
            // ...
            if (_reader.CheckNext1('.') && _reader.CheckNext1('.')) {
                ExpectedType();
                return TK_NAME;
            }
            return ReadRest();
        }
        default: {
            auto ch = _reader.GetCurrentChar();
            if (IsIdStart(ch)) {
                ReadId();
                if (_reader.GetCurrentChar() != '?') {
                    ExpectedType();
                }
                return TK_NAME;
            } else if (IsWhitespace(ch)) {
                return ReadWhitespace();
            }

            return ReadRest();
        }
    }
}

LuaTokenKind LuaDocLexer::ReadTagExpectedId() {
    auto ch = _reader.GetCurrentChar();
    if (IsWhitespace(ch)) {
        return ReadWhitespace();
    } else {
        _reader.EatWhile([](auto c) { return !LuaDocLexer::IsWhitespace(c); });
        return TK_NAME;
    }
}

LuaTokenKind LuaDocLexer::ReadTagDiagnostic() {
    switch (_reader.GetCurrentChar()) {
        case ':': {
            _reader.SaveAndNext();
            return TK_COLON;
        }
        case ',': {
            _reader.SaveAndNext();
            return TK_COMMA;
        }
        default: {
            auto ch = _reader.GetCurrentChar();
            if (IsWhitespace(ch)) {
                return ReadWhitespace();
            } else if (IsIdStart(ch)) {
                return ReadId();
            }
            return ReadRest();
        }
    }
}

LuaTokenKind LuaDocLexer::ReadType() {
    switch (_reader.GetCurrentChar()) {
        case '[': {
            _reader.SaveAndNext();
            if (_reader.GetCurrentChar() == ']') {
                _reader.SaveAndNext();
                return TK_ARR;
            }
            _typeLevel++;
            return TK_LBRACKET;
        }
        case ']': {
            _reader.SaveAndNext();
            _typeLevel--;
            return TK_RBRACKET;
        }
        case '<': {
            _reader.SaveAndNext();
            _typeLevel++;
            return TK_LT;
        }
        case '>': {
            _reader.SaveAndNext();
            _typeLevel--;
            return TK_GT;
        }
        case '\"':
        case '\'': {
            return ReadString();
        }
        case '.': {
            _reader.SaveAndNext();
            // ...
            if (_reader.CheckNext1('.') && _reader.CheckNext1('.')) {
                return TK_NAME;
            }
            return ReadRest();
        }
        case '(': {
            _reader.SaveAndNext();
            _typeLevel++;
            return TK_LPARN;
        }
        case ')': {
            _reader.SaveAndNext();
            _typeLevel--;
            return TK_RPARN;
        }
        case '{': {
            _reader.SaveAndNext();
            _typeLevel++;
            return TK_LCURLY;
        }
        case '}': {
            _reader.SaveAndNext();
            _typeLevel--;
            return TK_RCURLY;
        }
        case ',': {
            _reader.SaveAndNext();
            _typeReq = true;
            return TK_COMMA;
        }
        case ';': {
            _reader.SaveAndNext();
            _typeReq = true;
            return TK_SEMI;
        }
        case '|': {
            _reader.SaveAndNext();
            _typeReq = true;
            return TK_OR;
        }
        case ':': {
            _reader.SaveAndNext();
            _typeReq = true;
            return TK_COLON;
        }
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            return ReadNumeral();
        }
        default: {
            auto ch = _reader.GetCurrentChar();
            if (IsIdStart(ch)) {
                if (_typeReq || _typeLevel > 0) {
                    _typeReq = false;
                    ReadId();
                    auto id = _reader.GetSaveText();
                    if (id == "fun") {
                        return TK_FUN;
                    } else {
                        return TK_NAME;
                    }
                }
            } else if (IsWhitespace(ch)) {
                return ReadWhitespace();
            }
            PopState();
            _typeLevel = 0;
            return TK_RETRY;
        }
    }
}

LuaTokenKind LuaDocLexer::ReadCommentString() {
    _reader.EatWhile([](auto c) -> bool { return c != '\n' && c != '\r'; });
    return TK_COMMENT_CONTENT;
}

LuaTokenKind LuaDocLexer::ReadId() {
    _reader.EatWhile(IsIdContinue);
    return TK_NAME;
}

LuaTokenKind LuaDocLexer::ReadWhitespace() {
    _reader.EatWhile(IsWhitespace);
    return TK_WS;
}

LuaTokenKind LuaDocLexer::ReadRest() {
    _reader.EatWhile([](auto c) -> bool { return true; });
    return TK_REST;
}

void LuaDocLexer::ExpectedType() {
    _typeReq = true;
    PushState(State::Type);
}

LuaTokenKind LuaDocLexer::ReadString() {
    auto delimiter = _reader.GetCurrentChar();
    _reader.SaveAndNext();
    while (_reader.GetCurrentChar() != delimiter) {
        switch (_reader.GetCurrentChar()) {
            case EOF_CHAR:
            case '\n':
            case '\r': {
                TokenError("unfinished string", _reader.GetPos());
                goto endLoop;
            }
            case '\\': {
                _reader.SaveAndNext();
                break;
            }
            default: {
                break;
            }
        }
        _reader.SaveAndNext();
    }
endLoop:
    return TK_STRING;
}

LuaTokenKind LuaDocLexer::ReadNumeral() {
    int first = _reader.GetCurrentChar();
    const char *expo = "Ee";
    _reader.SaveAndNext();

    if (first == '0' && _reader.CheckNext2("xX")) /* hexadecimal? */
    {
        expo = "Pp";
    }

    for (;;) {
        if (_reader.CheckNext2(expo)) /* exponent mark? */
        {
            _reader.CheckNext2("-+");                                                      /* optional exponent sign */
        } else if (lisxdigit(_reader.GetCurrentChar()) || _reader.GetCurrentChar() == '.') /* '%x|%.' */
        {
            _reader.SaveAndNext();
        } else {
            break;
        }
    }

    return TK_NUMBER;
}

void LuaDocLexer::PushState(LuaDocLexer::State state) {
    _stateStack.push(state);
}

void LuaDocLexer::PopState() {
    if (!_stateStack.empty()) {
        _stateStack.pop();
    }
}

void LuaDocLexer::ChangeState(LuaDocLexer::State state) {
    if (!_stateStack.empty()) {
        _stateStack.top() = state;
    }
}

void LuaDocLexer::TokenError(std::string_view message, TextRange range) {
    _errors.emplace_back(message, range);
}

void LuaDocLexer::TokenError(std::string_view message, std::size_t offset) {
    TokenError(message, TextRange(offset, 0));
}

std::vector<LuaSyntaxError> &LuaDocLexer::GetErrors() {
    return _errors;
}

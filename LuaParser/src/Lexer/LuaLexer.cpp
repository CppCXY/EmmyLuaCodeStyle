#include "LuaParser/Lexer/LuaLexer.h"
#include <limits>
#include "LuaParser/Lexer/LuaDefine.h"
#include "LuaParser/Lexer/LuaIdentify.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "Util/format.h"
#include "Util/Utf8.h"
#include "LuaParser/File/LuaFile.h"

std::map<std::string, LuaTokenKind, std::less<>> LuaLexer::LuaReserved = {
        {"and",      TK_AND},
        {"break",    TK_BREAK},
        {"do",       TK_DO},
        {"else",     TK_ELSE},
        {"elseif",   TK_ELSEIF},
        {"end",      TK_END},
        {"false",    TK_FALSE},
        {"for",      TK_FOR},
        {"function", TK_FUNCTION},
        {"goto",     TK_GOTO},
        {"if",       TK_IF},
        {"in",       TK_IN},
        {"local",    TK_LOCAL},
        {"nil",      TK_NIL},
        {"not",      TK_NOT},
        {"or",       TK_OR},
        {"repeat",   TK_REPEAT},
        {"return",   TK_RETURN},
        {"then",     TK_THEN},
        {"true",     TK_TRUE},
        {"until",    TK_UNTIL},
        {"while",    TK_WHILE},
        {"//",       TK_IDIV},
        {"..",       TK_CONCAT},
        {"...",      TK_DOTS},
        {"==",       TK_EQ},
        {">=",       TK_GE},
        {"<=",       TK_LE},
        {"~=",       TK_NE},
        {"<<",       TK_SHL},
        {">>",       TK_SHR},
        {"::",       TK_DBCOLON}
};

LuaLexer::LuaLexer(std::shared_ptr<LuaFile> file)
        :
        _linenumber(0),
        _supportNonStandardSymbol(false),
        _reader(file->GetSource()),
        _file(file) {
}

bool LuaLexer::Parse() {
    _file->Reset();
    while (true) {
        auto type = Lex();
        if (type == TK_EOF) {
            break;
        }

        _tokens.emplace_back(type, _reader.GetTokenRange());
        if (!_errors.empty()) {
            _file->SetTotalLine(_linenumber);
            _file->UpdateLineInfo(_linenumber);
            return false;
        }
    }

    _file->SetTotalLine(_linenumber);
    return true;
}

std::vector<LuaTokenError> &LuaLexer::GetErrors() {
    return _errors;
}

bool LuaLexer::HasError() const {
    return !_errors.empty();
}

std::shared_ptr<LuaFile> LuaLexer::GetFile() {
    return _file;
}

std::vector<LuaToken> &LuaLexer::GetTokens() {
    return _tokens;
}

LuaTokenKind LuaLexer::Lex() {
    _reader.ResetBuffer();

    for (;;) {
        int ch = _reader.GetCurrentChar();
        switch (ch) {
            case '\n':
            case '\r': {
                IncLinenumber();
                break;
            }
            case ' ':
            case '\f':
            case '\t':
            case '\v': {
                _reader.NextChar();
                break;
            }
            case '-': {
                _reader.SaveAndNext();
                if (_reader.GetCurrentChar() != '-') {
                    if (_supportNonStandardSymbol && _reader.CheckNext1('=')) {
                        return '=';
                    }
                    return '-';
                }
                // is comment
                _reader.SaveAndNext();

                LuaTokenKind type = TK_SHORT_COMMENT;
                if (_reader.GetCurrentChar() == '[') {
                    std::size_t sep = SkipSep();
                    if (sep >= 2) {
                        ReadLongString(sep);
                        return TK_LONG_COMMENT;
                    }
                } else if (_reader.GetCurrentChar() == '-') {
                    _reader.SaveAndNext();
//                    type = TK_DOC_COMMENT;
                }

                // is short comment
                while (!CurrentIsNewLine() && _reader.GetCurrentChar() != EOZ) {
                    _reader.SaveAndNext();
                }

                return type;
            }
            case '+': {
                _reader.SaveAndNext();
                if (_supportNonStandardSymbol && _reader.CheckNext1('=')) {
                    return '=';
                }
                return '+';
            }
            case '[': {
                std::size_t sep = SkipSep();
                if (sep >= 2) {
                    ReadLongString(sep);
                    return TK_LONG_STRING;
                } else if (sep == 0) {
                    TokenError("invalid long string delimiter",
                               TextRange(_reader.GetPos(), _reader.GetPos()));
                    return TK_LONG_STRING;
                }
                return '[';
            }
            case '=': {
                _reader.SaveAndNext();
                if (_reader.CheckNext1('=')) {
                    return TK_EQ;
                } else {
                    return '=';
                }
            }
            case '<': {
                _reader.SaveAndNext();
                if (_reader.CheckNext1('=')) {
                    return TK_LE;
                } else if (_reader.CheckNext1('<')) {
                    return TK_SHL;
                } else {
                    return '<';
                }
            }
            case '>': {
                _reader.SaveAndNext();
                if (_reader.CheckNext1('=')) {
                    return TK_GE;
                } else if (_reader.CheckNext1('>')) {
                    return TK_SHR;
                } else {
                    return '>';
                }
            }
            case '/': {
                _reader.SaveAndNext();
                if (_reader.CheckNext1('/')) {
                    return TK_IDIV;
                } else {
                    return '/';
                }
            }
            case '~': {
                _reader.SaveAndNext();
                if (_reader.CheckNext1('=')) {
                    return TK_NE;
                } else {
                    return '~';
                }
            }
            case ':': {
                _reader.SaveAndNext();
                if (_reader.CheckNext1(':')) {
                    return TK_DBCOLON;
                } else {
                    return ':';
                }
            }
            case '"':
            case '\'':
                // extend support
            case '`': {
                ReadString(ch);
                return TK_STRING;
            }
            case '.': {
                _reader.SaveAndNext();
                if (_reader.CheckNext1('.')) {
                    if (_reader.CheckNext1('.')) {
                        return TK_DOTS; /* '...' */
                    } else {
                        return TK_CONCAT;
                    }
                } else if (!lisdigit(_reader.GetCurrentChar())) {
                    return '.';
                } else {
                    return ReadNumeral();
                }
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
            case EOZ: {
                return TK_EOF;
            }
            case '#': {
                _reader.SaveAndNext();
                // 只认为第一行的才是shebang
                if (_linenumber == 0 && _tokens.empty()) {
                    // shebang
                    while (!CurrentIsNewLine() && _reader.GetCurrentChar() != EOZ) {
                        _reader.SaveAndNext();
                    }

                    return TK_SHEBANG;
                }
                return '#';
            }
            default: {
                if (lislalpha(_reader.GetCurrentChar())) /* identifier or reserved word? */
                {
                    do {
                        _reader.SaveAndNext();
                    } while (lislalnum(_reader.GetCurrentChar()));

                    auto text = _reader.GetSaveText();

                    if (IsReserved(text)) {
                        return LuaReserved.find(text)->second;
                    } else {
                        return TK_NAME;
                    }
                } else /* single-char tokens ('+', '*', '%', '{', '}', ...) */
                {
                    int c = _reader.GetCurrentChar();
                    _reader.SaveAndNext();
                    return c;
                }
            }
        }
    }
}


/* LUA_NUMBER */
/*
** This function is quite liberal in what it accepts, as 'luaO_str2num'
** will reject ill-formed numerals. Roughly, it accepts the following
** pattern:
**
**   %d(%x|%.|([Ee][+-]?))* | 0[Xx](%x|%.|([Pp][+-]?))*
**
** The only tricky part is to accept [+-] only after a valid exponent
** mark, to avoid reading '3-4' or '0xe+1' as a single number.
**
** The caller might have already read an initial dot.
*/
LuaTokenKind LuaLexer::ReadNumeral() {
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
            _reader.CheckNext2("-+"); /* optional exponent sign */
        } else if (lisxdigit(_reader.GetCurrentChar()) || _reader.GetCurrentChar() == '.') /* '%x|%.' */
        {
            _reader.SaveAndNext();
        } else {
            break;
        }
    }

    // fix bug:这里不能使用lislalpha,否则会错误的解析下一个unicode字符
    if (std::isalpha(_reader.GetCurrentChar())) /* is numeral touching a letter? */
    {
        do {
            _reader.SaveAndNext();
        } while (std::isalpha(_reader.GetCurrentChar()));
    }

    return TK_NUMBER;
}

/*
** read a sequence '[=*[' or ']=*]', leaving the last bracket. If
** sequence is well formed, return its number of '='s + 2; otherwise,
** return 1 if it is a single bracket (no '='s and no 2nd bracket);
** otherwise (an unfinished '[==...') return 0.
*/
std::size_t LuaLexer::SkipSep() {
    std::size_t count = 0;
    int ch = _reader.GetCurrentChar();

    _reader.SaveAndNext();

    while (_reader.GetCurrentChar() == '=') {
        _reader.SaveAndNext();
        count++;
    }

    return _reader.GetCurrentChar() == ch
           ? count + 2
           : (count == 0)
             ? 1
             : 0;
}

void LuaLexer::ReadLongString(std::size_t sep) {
    _reader.SaveAndNext();

    if (CurrentIsNewLine()) {
        IncLinenumber();
    }

    for (;;) {
        switch (_reader.GetCurrentChar()) {
            case EOZ: {
                TokenError("unfinished long string starting", TextRange(_reader.GetPos(), _reader.GetPos()));
                return;
            }
            case ']': {
                if (SkipSep() == sep) {
                    _reader.SaveAndNext();
                    return;
                }
                break;
            }
            case '\n':
            case '\r': {
                _reader.Save();
                IncLinenumber();
                break;
            }
            default: {
                _reader.SaveAndNext();
            }
        }
    }
}

void LuaLexer::ReadString(int del) {
    _reader.SaveAndNext();
    while (_reader.GetCurrentChar() != del) {
        switch (_reader.GetCurrentChar()) {
            case EOZ:
            case '\n':
            case '\r': {
                TokenError("unfinished string", TextRange(_reader.GetPos(), _reader.GetPos()));
                return;
            }
            case '\\': {
                _reader.SaveAndNext();

                switch (_reader.GetCurrentChar()) {
                    case EOZ:
                        TokenError("unfinished string", TextRange(_reader.GetPos(), _reader.GetPos()));
                        return;
                    case 'z': {
                        _reader.SaveAndNext();
                        while (lisspace(_reader.GetCurrentChar())) {
                            if (CurrentIsNewLine()) {
                                IncLinenumber();
                            } else {
                                _reader.SaveAndNext();
                            }
                        }
                        goto no_save;
                    }
                    case '\r':
                    case '\n': {
                        if (CurrentIsNewLine()) {
                            IncLinenumber();
                        }
                        goto no_save;
                    }
                }
                break;
            }
        }
        _reader.SaveAndNext();
        // 空语句
        no_save:;
    }
    _reader.SaveAndNext();
}


void LuaLexer::IncLinenumber() {
    int old = _reader.GetCurrentChar();

    _reader.NextChar();

    if (CurrentIsNewLine() && _reader.GetCurrentChar() != old) {
        _reader.NextChar(); /* skip '\n\r' or '\r\n' */
        _file->SetEndOfLineState(EndOfLine::CRLF);
    } else if (old == '\n') {
        _file->SetEndOfLineState(EndOfLine::LF);
    } else {
        _file->SetEndOfLineState(EndOfLine::CR);
    }

    if (++_linenumber >= std::numeric_limits<int>::max()) {
        return;
    }

    _file->PushLine(_reader.GetPos());
}

bool LuaLexer::CurrentIsNewLine() {
    int ch = _reader.GetCurrentChar();
    return ch == '\n' || ch == '\r';
}

bool LuaLexer::IsReserved(std::string_view text) {
    return LuaReserved.find(text) != LuaReserved.end();
}

void LuaLexer::TokenError(std::string_view message, TextRange range) {
    _errors.emplace_back(message, range, 0);
}

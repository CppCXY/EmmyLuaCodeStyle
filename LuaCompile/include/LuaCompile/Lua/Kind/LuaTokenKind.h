#pragma once

#include <cstdint>
#include <string_view>
//#include <fmt/format.h>

enum class LuaTokenKind {
    TK_AND,
    TK_BREAK,
    TK_DO,
    TK_ELSE,
    TK_ELSEIF,
    TK_END,
    TK_FALSE,
    TK_FOR,
    TK_FUNCTION,
    TK_GOTO,
    TK_IF,
    TK_IN,
    TK_LOCAL,
    TK_NIL,
    TK_NOT,
    TK_OR,
    TK_REPEAT,
    TK_RETURN,
    TK_THEN,
    TK_TRUE,
    TK_UNTIL,
    TK_WHILE,

    TK_WS,           // whitespace
    TK_LBRACKET,     // '['
    TK_RBRACKET,     // ']'
    TK_DOT,          // '.'
    TK_GT,           // '>'
    TK_LT,           // '<'
    TK_TILDE,        // '~'
    TK_COLON,        // ':'
    TK_PLUS,         // '+'
    TK_MINUS,        // '-'
    TK_MULT,         // '*'
    TK_MOD,          // '%'
    TK_LPARN,        // '('
    TK_RPARN,        // ')'
    TK_LCURLY,       // '{'
    TK_RCURLY,       // '}'
    TK_GETN,         // '#'
    TK_COMMA,        // ','
    TK_SEMI,         // ';'
    TK_EXP,          // '^'
    TK_BIT_AND,      // '&'
    TK_BIT_OR,       // '|'
    TK_DIV,          // '/'
    TK_IDIV,         // '//'
    TK_CONCAT,       // '..'
    TK_DOTS,         // '...'
    TK_EQ,           // '='
    TK_EQEQ,         // '=='
    TK_GE,           // '>='
    TK_LE,           // '<='
    TK_NE,           // '~='
    TK_SHL,          // '<<'
    TK_SHR,          // '>>'
    TK_DBCOLON,      // '::'
    TK_FLT,          // float number, not use
    TK_NUMBER,       // number
    TK_NAME,         // ID
    TK_STRING,       // short string
    TK_LONG_STRING,  // long string
    TK_SHORT_COMMENT,//
    TK_LONG_COMMENT, //
    TK_SHEBANG,      //shebang
    TK_EOF,          // end of file
    TK_ERR,          // 错误Token

    // lua doc special
    TK_AT,            // '@'
    TK_DASH_DASH_DASH,// '---'
    TK_DASHES,        // '--'
    TK_ARR,           // '[]'
    TK_NULLABLE,      // '?'
    TK_FUN,           // 'fun'
    TK_RETRY,         // trivial token
    TK_REST,
    TK_COMMENT_CONTENT,
    TK_ACCESS,
    TK_TAG_CLASS,
    TK_TAG_ENUM,
    TK_TAG_INTERFACE,
    TK_TAG_FIELD,
    TK_TAG_GENERIC,
    TK_TAG_TYPE,
    TK_TAG_PARAM,
    TK_TAG_ALIAS,
    TK_TAG_PUBLIC,
    TK_TAG_PRIVATE,
    TK_TAG_PROTECTED,
    TK_TAG_OVERLOAD,
    TK_TAG_OVERRIDE,
    TK_TAG_SEE,
    TK_TAG_LANGUAGE,
    TK_TAG_SINCE,
    TK_TAG_VERSION,
    TK_TAG_DEPRECATED,
    TK_TAG_DIAGNOSTIC,

    TK_UNKNOWN,
};
namespace detail::debug {
constexpr std::string_view GetLuaTokenKindDebugName(LuaTokenKind kind) {
    switch (kind) {
        case LuaTokenKind::TK_AND:
            return "TK_AND";
        case LuaTokenKind::TK_BREAK:
            return "TK_BREAK";
        case LuaTokenKind::TK_DO:
            return "TK_DO";
        case LuaTokenKind::TK_ELSE:
            return "TK_ELSE";
        case LuaTokenKind::TK_ELSEIF:
            return "TK_AND";
        case LuaTokenKind::TK_END:
            return "TK_END";
        case LuaTokenKind::TK_FALSE:
            return "TK_FALSE";
        case LuaTokenKind::TK_FOR:
            return "TK_FOR";
        case LuaTokenKind::TK_FUNCTION:
            return "TK_FUNCTION";
        case LuaTokenKind::TK_GOTO:
            return "TK_GOTO";
        case LuaTokenKind::TK_IF:
            return "TK_IF";
        case LuaTokenKind::TK_IN:
            return "TK_IN";
        case LuaTokenKind::TK_LOCAL:
            return "TK_LOCAL";
        case LuaTokenKind::TK_NIL:
            return "TK_NIL";
        case LuaTokenKind::TK_NOT:
            return "TK_NOT";
        case LuaTokenKind::TK_OR:
            return "TK_OR";
        case LuaTokenKind::TK_REPEAT:
            return "TK_REPEAT";
        case LuaTokenKind::TK_RETURN:
            return "TK_RETURN";
        case LuaTokenKind::TK_THEN:
            return "TK_THEN";
        case LuaTokenKind::TK_TRUE:
            return "TK_TRUE";
        case LuaTokenKind::TK_UNTIL:
            return "TK_UNTIL";
        case LuaTokenKind::TK_WHILE:
            return "TK_WHILE";
        case LuaTokenKind::TK_WS:
            return "TK_WS";
        case LuaTokenKind::TK_LBRACKET:
            return "TK_LBRACKET";
        case LuaTokenKind::TK_RBRACKET:
            return "TK_RBRACKET";
        case LuaTokenKind::TK_DOT:
            return "TK_DOT";
        case LuaTokenKind::TK_GT:
            return "TK_GT";
        case LuaTokenKind::TK_LT:
            return "TK_LT";
        case LuaTokenKind::TK_TILDE:
            return "TK_TILED";
        case LuaTokenKind::TK_COLON:
            return "TK_COLON";
        case LuaTokenKind::TK_PLUS:
            return "TK_PLUS";
        case LuaTokenKind::TK_MINUS:
            return "TK_MINUS";
        case LuaTokenKind::TK_MULT:
            return "TK_MULT";
        case LuaTokenKind::TK_MOD:
            return "TK_MOD";
        case LuaTokenKind::TK_LPARN:
            return "TK_LPARN";
        case LuaTokenKind::TK_RPARN:
            return "TK_RPARN";
        case LuaTokenKind::TK_LCURLY:
            return "TK_LCURLY";
        case LuaTokenKind::TK_RCURLY:
            return "TK_RCURLY";
        case LuaTokenKind::TK_GETN:
            return "TK_GETN";
        case LuaTokenKind::TK_COMMA:
            return "TK_COMMA";
        case LuaTokenKind::TK_SEMI:
            return "TK_SEMI";
        case LuaTokenKind::TK_EXP:
            return "TK_EXP";
        case LuaTokenKind::TK_BIT_AND:
            return "TK_BIT_AND";
        case LuaTokenKind::TK_BIT_OR:
            return "TK_BIT_OR";
        case LuaTokenKind::TK_DIV:
            return "TK_DIV";
        case LuaTokenKind::TK_IDIV:
            return "TK_IDIV";
        case LuaTokenKind::TK_CONCAT:
            return "TK_CONCAT";
        case LuaTokenKind::TK_DOTS:
            return "TK_DOTS";
        case LuaTokenKind::TK_EQ:
            return "TK_EQ";
        case LuaTokenKind::TK_EQEQ:
            return "TK_EQEQ";
        case LuaTokenKind::TK_GE:
            return "TK_GE";
        case LuaTokenKind::TK_LE:
            return "TK_LE";
        case LuaTokenKind::TK_NE:
            return "TK_NE";
        case LuaTokenKind::TK_SHL:
            return "TK_SHL";
        case LuaTokenKind::TK_SHR:
            return "TK_SHR";
        case LuaTokenKind::TK_DBCOLON:
            return "TK_DBCOLON";
        case LuaTokenKind::TK_FLT:
            return "TK_FLT";
        case LuaTokenKind::TK_NUMBER:
            return "TK_NUMBER";
        case LuaTokenKind::TK_NAME:
            return "TK_NAME";
        case LuaTokenKind::TK_STRING:
            return "TK_STRING";
        case LuaTokenKind::TK_LONG_STRING:
            return "TK_STRING";
        case LuaTokenKind::TK_SHORT_COMMENT:
            return "TK_SHORT_COMMENT";
        case LuaTokenKind::TK_LONG_COMMENT:
            return "TK_LONG_COMMENT";
        case LuaTokenKind::TK_SHEBANG:
            return "TK_SHEBANG";
        case LuaTokenKind::TK_EOF:
            return "TK_EOF";
        case LuaTokenKind::TK_ERR:
            return "TK_ERR";
        case LuaTokenKind::TK_UNKNOWN:
            return "TK_UNKNOWN";
        case LuaTokenKind::TK_AT:
            return "TK_AT";
        case LuaTokenKind::TK_DASH_DASH_DASH:
            return "TK_DASH_DASH_DASH";
        case LuaTokenKind::TK_DASHES:
            return "TK_DASHES";
        case LuaTokenKind::TK_ARR:
            return "TK_ARR";
        case LuaTokenKind::TK_NULLABLE:
            return "TK_NULLABLE";
        case LuaTokenKind::TK_FUN:
            return "TK_FUN";
        case LuaTokenKind::TK_RETRY:
            return "TK_RETRY";
        case LuaTokenKind::TK_REST:
            return "TK_REST";
        case LuaTokenKind::TK_COMMENT_CONTENT:
            return "TK_COMMENT_CONTENT";
        case LuaTokenKind::TK_ACCESS:
            return "TK_ACCESS";
        case LuaTokenKind::TK_TAG_CLASS:
            return "TK_TAG_CLASS";
        case LuaTokenKind::TK_TAG_ENUM:
            return "TK_TAG_ENUM";
        case LuaTokenKind::TK_TAG_INTERFACE:
            return "TK_TAG_INTERFACE";
        case LuaTokenKind::TK_TAG_FIELD:
            return "TK_TAG_FIELD";
        case LuaTokenKind::TK_TAG_GENERIC:
            return "TK_TAG_GENERIC";
        case LuaTokenKind::TK_TAG_TYPE:
            return "TK_TAG_TYPE";
        case LuaTokenKind::TK_TAG_PARAM:
            return "TK_TAG_PARAM";
        case LuaTokenKind::TK_TAG_ALIAS:
            return "TK_TAG_ALIAS";
        case LuaTokenKind::TK_TAG_PUBLIC:
            return "TK_TAG_PUBLIC";
        case LuaTokenKind::TK_TAG_PRIVATE:
            return "TK_TAG_PRIVATE";
        case LuaTokenKind::TK_TAG_PROTECTED:
            return "TK_TAG_PROTECTED";
        case LuaTokenKind::TK_TAG_OVERLOAD:
            return "TK_TAG_OVERLOAD";
        case LuaTokenKind::TK_TAG_OVERRIDE:
            return "TK_TAG_OVERRIDE";
        case LuaTokenKind::TK_TAG_SEE:
            return "TK_TAG_SEE";
        case LuaTokenKind::TK_TAG_LANGUAGE:
            return "TK_TAG_LANGUAGE";
        case LuaTokenKind::TK_TAG_SINCE:
            return "TK_TAG_SINCE";
        case LuaTokenKind::TK_TAG_VERSION:
            return "TK_TAG_VERSION";
        case LuaTokenKind::TK_TAG_DEPRECATED:
            return "TK_TAG_DEPRECATED";
        case LuaTokenKind::TK_TAG_DIAGNOSTIC:
            return "TK_TAG_DIAGNOSTIC";
        default: {
            return "UNKNOWN";
        }
    }
}
}// namespace detail::debug

//template<>
//struct fmt::formatter<LuaTokenKind> {
//    constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
//        // Parse the presentation format and store it in the formatter:
//        auto it = ctx.begin(), end = ctx.end();
//
//        // Check if reached the end of the range:
//        while (it != end && *it != '}') { it++; }
//
//        // Return an iterator past the end of the parsed range:
//        return it;
//    }
//
//    // Formats the point p using the parsed format specification (presentation)
//    // stored in this formatter.
//    template<typename FormatContext>
//    auto format(const LuaTokenKind &p, FormatContext &ctx) const -> decltype(ctx.out()) {
//        // ctx.out() is an output iterator to write to.
//        return fmt::format_to(ctx.out(), "{}", ::detail::debug::GetLuaTokenKindDebugName(p));
//    }
//};

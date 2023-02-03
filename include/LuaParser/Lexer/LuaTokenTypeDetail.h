#pragma once

#include <climits>

const int FIRST_RESERVED = UCHAR_MAX + 1;

enum LuaTokenTypeDetail : int
{
	/* terminal symbols denoted by reserved words */
	TK_AND = FIRST_RESERVED,
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
	/* other terminal symbols */
	TK_IDIV,
	TK_CONCAT,
	TK_DOTS,
	TK_EQ,
	TK_GE,
	TK_LE,
	TK_NE,
	TK_SHL,
	TK_SHR,
	TK_DBCOLON,
	TK_FLT,
	TK_NUMBER,
	TK_NAME,
	TK_STRING,
    TK_LONG_STRING,
	TK_SHORT_COMMENT,// 短注释
	TK_DOC_COMMENT, // emmylua doc
	TK_LONG_COMMENT, //长注释
	TK_SHEBANG, //shebang
    TK_EOF, // 文件末尾
    TK_ERR, // 错误Token
	// DOC
	TK_DOC_TAG_FORMAT,
	TK_DOC_DISABLE_NEXT,
	TK_DOC_DISABLE,

	TK_UNKNOWN, //对自定义tokenParser来说是未知
};

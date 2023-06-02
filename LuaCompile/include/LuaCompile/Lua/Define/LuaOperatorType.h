#pragma once

#include "LuaCompile/Lua/Kind/LuaTokenKind.h"

// 取名都来自lua 源代码
enum class UnOpr {
    OPR_MINUS,
    OPR_BNOT,
    OPR_NOT,
    OPR_LEN,
    OPR_NOUNOPR
};

/*
** grep "ORDER OPR" if you change these enums  (ORDER OP)
*/
enum class BinOpr {
    /* arithmetic operators */
    OPR_ADD,
    OPR_SUB,
    OPR_MUL,
    OPR_MOD,
    OPR_POW,
    OPR_DIV,
    OPR_IDIV,
    /* bitwise operators */
    OPR_BAND,
    OPR_BOR,
    OPR_BXOR,
    OPR_SHL,
    OPR_SHR,
    /* string operator */
    OPR_CONCAT,
    /* comparison operators */
    OPR_EQ,
    OPR_LT,
    OPR_LE,
    OPR_NE,
    OPR_GT,
    OPR_GE,
    /* logical operators */
    OPR_AND,
    OPR_OR,
    OPR_NOBINOPR
};

namespace detail::lua_opr {
UnOpr GetUnaryOperator(LuaTokenKind op);

BinOpr GetBinaryOperator(LuaTokenKind op);
}// namespace detail::lua_opr
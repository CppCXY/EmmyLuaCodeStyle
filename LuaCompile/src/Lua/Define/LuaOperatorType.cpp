#include "LuaCompile/Lua/Define/LuaOperatorType.h"

using enum LuaTokenKind;

UnOpr detail::lua_opr::GetUnaryOperator(LuaTokenKind op) {
    switch (op) {
        case TK_NOT: {
            return UnOpr::OPR_NOT;
        }
        case TK_MINUS: {
            return UnOpr::OPR_MINUS;
        }
        case TK_TILDE: {
            return UnOpr::OPR_BNOT;
        }
        case TK_GETN: {
            return UnOpr::OPR_LEN;
        }
        default: {
            return UnOpr::OPR_NOUNOPR;
        }
    }
}

BinOpr detail::lua_opr::GetBinaryOperator(LuaTokenKind op) {
    switch (op) {
        case TK_PLUS:
            return BinOpr::OPR_ADD;
        case TK_MINUS:
            return BinOpr::OPR_SUB;
        case TK_MULT:
            return BinOpr::OPR_MUL;
        case TK_MOD:
            return BinOpr::OPR_MOD;
        case TK_EXP:
            return BinOpr::OPR_POW;
        case TK_DIV:
            return BinOpr::OPR_DIV;
        case TK_IDIV:
            return BinOpr::OPR_IDIV;
        case TK_BIT_AND:
            return BinOpr::OPR_BAND;
        case TK_BIT_OR:
            return BinOpr::OPR_BOR;
        case TK_TILDE:
            return BinOpr::OPR_BXOR;
        case TK_SHL:
            return BinOpr::OPR_SHL;
        case TK_SHR:
            return BinOpr::OPR_SHR;
        case TK_CONCAT:
            return BinOpr::OPR_CONCAT;
        case TK_NE:
            return BinOpr::OPR_NE;
        case TK_EQEQ:
            return BinOpr::OPR_EQ;
        case TK_LT:
            return BinOpr::OPR_LT;
        case TK_LE:
            return BinOpr::OPR_LE;
        case TK_GT:
            return BinOpr::OPR_GT;
        case TK_GE:
            return BinOpr::OPR_GE;
        case TK_AND:
            return BinOpr::OPR_AND;
        case TK_OR:
            return BinOpr::OPR_OR;
        default:
            return BinOpr::OPR_NOBINOPR;
    }
}

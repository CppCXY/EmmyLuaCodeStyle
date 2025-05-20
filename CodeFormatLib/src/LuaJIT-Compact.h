#ifndef LUAJIT_COMPACT_H
#define LUAJIT_COMPACT_H

#ifdef LUAJIT
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

int lua_isinteger(lua_State *L, int idx);

int luaL_len(lua_State *L, int idx);

int lua_geti(lua_State *L, int idx, lua_Integer n);
#endif
#endif //LUAJIT_COMPACT_H

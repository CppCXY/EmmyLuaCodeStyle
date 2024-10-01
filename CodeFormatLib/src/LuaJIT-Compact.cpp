#include "LuaJIT-Compact.h"

#ifdef LUAJIT
int lua_isinteger(lua_State *L, int idx) {
    if (lua_type(L, idx) == LUA_TNUMBER)
    {
        lua_Number n = lua_tonumber(L, idx);
        return n == (lua_Integer)n;
    }
    return 0;
}

int luaL_len(lua_State *L, int idx) {
    return lua_objlen(L, idx);
}

int lua_geti(lua_State *L, int idx, lua_Integer n) {
    lua_rawgeti(L, idx, n);
    return lua_type(L, -1);
}
#endif
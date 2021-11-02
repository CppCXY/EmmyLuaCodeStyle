#include "lua.hpp"
#include "CodeService/LuaFormatter.h"

#ifdef MSVC
#define EXPORT    __declspec(dllexport)
#else
#define EXPORT   
#endif


int format(lua_State* L)
{
	int top = lua_gettop(L);

	if (top == 0)
	{
		return 0;
	}

	if (lua_isstring(L, 1))
	{
		std::string buffer = lua_tostring(L, 1);
		auto parser = LuaParser::LoadFromBuffer(std::move(buffer));
		parser->BuildAstWithComment();

		if (!parser->GetErrors().empty())
		{
			lua_pushboolean(L, false);
			return 1;
		}
		LuaFormatOptions options;
		LuaFormatter formatter(parser, options);
		formatter.BuildFormattedElement();

		std::string formattedText = formatter.GetFormattedText();
		lua_pushboolean(L, true);
		lua_pushlstring(L, formattedText.c_str(), formattedText.size());
		return 2;
	}
	return 0;
}

static const luaL_Reg lib[] = {
	{"format",format},
	{nullptr, nullptr}
};


extern "C"
EXPORT int luaopen_code_format(lua_State* L)
{
	luaL_newlibtable(L, lib);
	luaL_setfuncs(L, lib, 0);
	return 1;
}

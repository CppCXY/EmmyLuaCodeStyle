#include "lua.hpp"
#include "LuaCodeFormat.h"

#ifdef MSVC
#define EXPORT    __declspec(dllexport)
#else
#define EXPORT   
#endif

int format(lua_State* L)
{
	int top = lua_gettop(L);

	if (top < 2)
	{
		return 0;
	}

	if (lua_isstring(L, 1) && lua_isstring(L, 2))
	{
		try
		{
			std::string filename = lua_tostring(L, 1);
			std::string text = lua_tostring(L, 2);
			auto formattedText = LuaCodeFormat::GetInstance().Reformat(filename, std::move(text));
			if (formattedText.empty())
			{
				lua_pushboolean(L, false);
				return 1;
			}
			lua_pushboolean(L, true);
			lua_pushlstring(L, formattedText.c_str(), formattedText.size());
			return 2;
		}
		catch (std::exception& e)
		{
			std::string err = e.what();
			lua_settop(L, top);
			lua_pushboolean(L, false);
			lua_pushlstring(L, err.c_str(), err.size());
			return 2;
		}
	}
	return 0;
}

enum class UpdateType
{
	Created = 1,
	Changed = 2,
	Deleted = 3
};

int range_format(lua_State* L)
{
	int top = lua_gettop(L);

	if (top < 4)
	{
		return 0;
	}

	if (lua_isstring(L, 1) && lua_isstring(L, 2) && lua_isinteger(L, 3) && lua_isinteger(L, 4))
	{
		try
		{
			std::string filename = lua_tostring(L, 1);
			std::string text = lua_tostring(L, 2);
			int startLine = lua_tointeger(L, 3);
			int endLine = lua_tointeger(L, 4);

			LuaFormatRange range(startLine, endLine);
			auto formattedText = LuaCodeFormat::GetInstance().RangeFormat(filename, range, std::move(text));
			if (formattedText.empty())
			{
				lua_pushboolean(L, false);
				return 1;
			}
			lua_pushboolean(L, true);
			lua_pushlstring(L, formattedText.c_str(), formattedText.size());
			lua_pushinteger(L, range.StartLine);
			lua_pushinteger(L, range.EndLine);

			return 4;
		}
		catch (std::exception& e)
		{
			std::string err = e.what();
			lua_settop(L, top);
			lua_pushboolean(L, false);
			lua_pushlstring(L, err.c_str(), err.size());
			return 2;
		}
	}
	return 0;
}

int update_config(lua_State* L)
{
	int top = lua_gettop(L);

	if (top < 3)
	{
		return 0;
	}

	if (lua_isinteger(L, 1) && lua_isstring(L, 2) && lua_isstring(L, 3))
	{
		try
		{
			auto type = static_cast<UpdateType>(lua_tointeger(L, 1));
			std::string workspaceUri = lua_tostring(L, 2);
			std::string configPath = lua_tostring(L, 3);
			switch (type)
			{
			case UpdateType::Created:
			case UpdateType::Changed:
				{
					LuaCodeFormat::GetInstance().UpdateCodeStyle(workspaceUri, configPath);
					break;
				}
			case UpdateType::Deleted:
				{
					LuaCodeFormat::GetInstance().RemoveCodeStyle(workspaceUri);
					break;
				}
			}

			lua_pushboolean(L, true);
			return 1;
		}
		catch (std::exception& e)
		{
			std::string err = e.what();
			lua_settop(L, top);
			lua_pushboolean(L, false);
			lua_pushlstring(L, err.c_str(), err.size());
			return 2;
		}
	}

	return 0;
}

static const luaL_Reg lib[] = {
	{"format", format},
	{"range_format", range_format},
	{"update_config", update_config},
	{nullptr, nullptr}
};

extern "C"
EXPORT int luaopen_code_format(lua_State* L)
{
	luaL_newlibtable(L, lib);
	luaL_setfuncs(L, lib, 0);
	return 1;
}

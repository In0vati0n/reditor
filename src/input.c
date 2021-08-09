/**
 * 输入相关
 *
 * Author: In0vati0n
 * Date: 2021/08/09
 */

#include "input.h"

static int lua_ctrlKey(lua_State *L)
{
    if (lua_isnumber(L, -1))
    {
        int key = lua_tonumber(L, -1);
        lua_pushnumber(L, CTRL_KEY(key));
    }
    else if (lua_isstring(L, -1))
    {
        const char *c = lua_tostring(L, -1);
        lua_pushnumber(L, CTRL_KEY(*c));
    }
    else
    {
        luaL_typeerror(L, 0, "number or string");
        return 0;
    }

    return 1;
}

int initInputLib(lua_State *L)
{
    lua_getglobal(L, RE_LUA_GLOBAL_NAME);

    lua_newtable(L);
    lua_setfield(L, -2, "input");

    lua_getfield(L, -1, "input");

    lua_pushcfunction(L, lua_ctrlKey);
    lua_setfield(L, -2, "ctrlKey");

    lua_pop(L, 1); // pop reditor.terminal_render table

    lua_pop(L, 1); // pop reditor table
    return 0;
}
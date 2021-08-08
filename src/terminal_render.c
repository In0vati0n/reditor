/**
 * 终端渲染相关逻辑
 *
 * Author: In0vati0n
 * Date: 2021/08/08
 */

#include "terminal_render.h"

#include <unistd.h>
#include <string.h>

void draw(const char *chars, int len)
{
    write(STDOUT_FILENO, chars, len);
}

int lua_draw(lua_State *L)
{
    const char *content = lua_tostring(L, -1);
    draw(content, strlen(content));
    return 0;
}

int initTerminalRenderLib(lua_State *L)
{
    lua_getglobal(L, RE_LUA_GLOBAL_NAME);

    lua_newtable(L);
    lua_setfield(L, -2, "terminal_render");

    lua_getfield(L, -1, "terminal_render");

    lua_pushcfunction(L, lua_draw);
    lua_setfield(L, -2, "draw");

    lua_pop(L, 1); // pop reditor.terminal_render table

    lua_pop(L, 1); // pop reditor table
    return 0;
}
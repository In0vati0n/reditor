/**
 * 输入相关
 *
 * Author: In0vati0n
 * Date: 2021/08/09
 */

#include <unistd.h>
#include <errno.h>

#include "input.h"

/**
 * 读取用户输入
 * 对 read() 的封装
 */
int ti_editorReadKey()
{
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (nread == -1 && errno != EAGAIN)
            die("read");
    }

    /**
     * 由于方向键输入会转换成 <esc>[x 的形式
     * PAGEUP PAGEDOWN 会转换成 <ecs>[5~ <ecs>[6~
     * 这里特殊处理一下
     */
    if (c == '\x1b')
    {
        char seq[3];

        if (read(STDIN_FILENO, &seq[0], 1) != 1)
            return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1)
            return '\x1b';

        if (seq[0] == '[')
        {
            // 检查第二个参数是否为数字
            if (seq[1] >= '0' && seq[1] <= '9')
            {
                if (read(STDIN_FILENO, &seq[2], 1) != 1)
                    return '\x1b';

                if (seq[2] == '~')
                {
                    switch (seq[1])
                    {
                    case '1':
                        return HOME_KEY;
                    case '3':
                        return DEL_KEY;
                    case '4':
                        return END_KEY;
                    case '5':
                        return PAGE_UP;
                    case '6':
                        return PAGE_DOWN;
                    case '7':
                        return HOME_KEY;
                    case '8':
                        return END_KEY;
                    }
                }
            }
            else
            {
                switch (seq[1])
                {
                case 'A':
                    return ARROW_UP;
                case 'B':
                    return ARROW_DOWN;
                case 'C':
                    return ARROW_RIGHT;
                case 'D':
                    return ARROW_LEFT;
                case 'H':
                    return HOME_KEY;
                case 'F':
                    return END_KEY;
                }
            }
        }
        else if (seq[0] == 'O')
        {
            switch (seq[1])
            {
            case 'H':
                return HOME_KEY;
            case 'F':
                return END_KEY;
            }
        }

        return '\x1b';
    }
    else
    {
        return c;
    }
}

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

static int lua_readKey(lua_State *L)
{
    lua_pushnumber(L, ti_editorReadKey());

    return 1;
}

int ti_initLib(lua_State *L)
{
    lua_getglobal(L, RE_LUA_GLOBAL_NAME);

    /*** input table ***/
    lua_newtable(L);
    lua_setfield(L, -2, "input");
    lua_getfield(L, -1, "input");

    lua_pushnumber(L, BACKSPACE);
    lua_setfield(L, -2, "BACKSPACE");
    lua_pushnumber(L, ARROW_LEFT);
    lua_setfield(L, -2, "ARROW_LEFT");
    lua_pushnumber(L, ARROW_RIGHT);
    lua_setfield(L, -2, "ARROW_RIGHT");
    lua_pushnumber(L, ARROW_UP);
    lua_setfield(L, -2, "ARROW_UP");
    lua_pushnumber(L, ARROW_DOWN);
    lua_setfield(L, -2, "ARROW_DOWN");
    lua_pushnumber(L, DEL_KEY);
    lua_setfield(L, -2, "DEL_KEY");
    lua_pushnumber(L, HOME_KEY);
    lua_setfield(L, -2, "HOME_KEY");
    lua_pushnumber(L, END_KEY);
    lua_setfield(L, -2, "END_KEY");
    lua_pushnumber(L, PAGE_UP);
    lua_setfield(L, -2, "PAGE_UP");
    lua_pushnumber(L, PAGE_DOWN);
    lua_setfield(L, -2, "PAGE_DOWN");

    lua_pop(L, 1); // pop reditor.input table

    /*** t_input table ***/
    lua_newtable(L);
    lua_setfield(L, -2, "t_input");
    lua_getfield(L, -1, "t_input");

    lua_pushcfunction(L, lua_ctrlKey);
    lua_setfield(L, -2, "ctrlKey");

    lua_pushcfunction(L, lua_readKey);
    lua_setfield(L, -2, "readKey");

    lua_pop(L, 1); // pop reditor.t_input table

    lua_pop(L, 1); // pop reditor table
    return 0;
}
/**
 * 终端渲染相关逻辑
 *
 * Author: In0vati0n
 * Date: 2021/08/08
 */

#include "terminal_render.h"

#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

/**
 * 绘制字符串
 */
static void draw(const char *chars, int len)
{
    write(STDOUT_FILENO, chars, len);
}

/**
 * 获取当前光标位置
 */
static int getCursorPosition(/* out */ int *rows, /* out */ int *cols)
{
    char buf[32];
    unsigned int i = 0;

    /**
     * 使用 n 命令获取当前命令行状态
     * 参看：http://vt100.net/docs/vt100-ug/chapter3.html#DSR
     *
     * 输出结果为一个转义序列
     * 形如 <ecs>[24;80R
     * 参看：http://vt100.net/docs/vt100-ug/chapter3.html#CPR
     */
    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
        return -1;

    while (i < sizeof(buf) - 1)
    {
        if (read(STDIN_FILENO, &buf[i], 1) != 1)
            break;
        // 判断输入是否结束
        if (buf[i] == 'R')
            break;
        i++;
    }
    buf[i] = '\0';

    // 检查是否有错误
    if (buf[0] != '\x1b' || buf[1] != '[')
        return -1;

    // 读取行、列数据
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2)
        return -1;

    return 0;
}

/**
 * 获得当前窗口尺寸
 */
static int getWindowSize(/* out */ int *rows, /* out */ int *cols)
{
    struct winsize ws;

    // 尝试通过 ioctl 获取当前终端尺寸
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
    {
        /**
         * 保底策略，将光标位置放置到右下角
         * 再通过查询当前光标位置来获取信息
         * 其中
         * - C：将光标移动到右侧 
         *      http://vt100.net/docs/vt100-ug/chapter3.html#CUF
         * - B：将光标移动到底部
         *      http://vt100.net/docs/vt100-ug/chapter3.html#CUD
         * - 999：保证光标能移动到右下角
         * 使用 C、B 命令能保证光标不会移动到屏幕边界外
         * 而 H 命令有可能将光标移动出去
         */
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
            return -1;

        return getCursorPosition(rows, cols);
    }
    else
    {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

/*** lua wrapper ***/

static int lua_draw(lua_State *L)
{
    const char *content = lua_tostring(L, -1);
    draw(content, strlen(content));
    return 0;
}

static int lua_getWindowSize(lua_State *L)
{
    int width, height;
    getWindowSize(&height, &width);

    lua_pushnumber(L, width);
    lua_pushnumber(L, height);

    return 2;
}

int initTerminalRenderLib(lua_State *L)
{
    lua_getglobal(L, RE_LUA_GLOBAL_NAME);

    lua_newtable(L);
    lua_setfield(L, -2, "terminal_render");

    lua_getfield(L, -1, "terminal_render");

    lua_pushcfunction(L, lua_draw);
    lua_setfield(L, -2, "draw");

    lua_pushcfunction(L, lua_getWindowSize);
    lua_setfield(L, -2, "getWindowSize");

    lua_pop(L, 1); // pop reditor.terminal_render table

    lua_pop(L, 1); // pop reditor table
    return 0;
}
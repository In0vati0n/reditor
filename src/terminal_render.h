/**
 * 终端渲染相关逻辑
 *
 * Author: In0vati0n
 * Date: 2021/08/08
 */

#ifndef __RE_TERMINAL_RENDER__
#define __RE_TERMINAL_RENDER__

#include "lua.h"

#include "common.h"

/**
 * 终端渲染
 */
RE_API void tr_draw(const char *chars, int len);

/**
 * 获取当前光标位置
 */
RE_API int tr_getCursorPosition(/* out */ int *rows, /* out */ int *cols);

/**
 * 获得当前窗口尺寸
 */
RE_API int tr_getWindowSize(/* out */ int *rows, /* out */ int *cols);

/**
 * 初始化终端 Lua 渲染库
 */
RE_API int tr_initLib(lua_State *L);

#endif // __RE_TERMINAL_RENDER__

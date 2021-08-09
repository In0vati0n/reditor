/**
 * 输入相关
 *
 * Author: In0vati0n
 * Date: 2021/08/09
 */

#ifndef __RE_INPUT__
#define __RE_INPUT__

#include "lua.h"

#include "common.h"

#define CTRL_KEY(k) ((k)&0x1f)

enum editorKey
{
    BACKSPACE = 127,
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    DEL_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN
};

RE_API int initInputLib(lua_State *L);

#endif // __RE_INPUT__
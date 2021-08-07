/**
 * 渲染相关逻辑
 *
 * Author: In0vati0n
 * Date: 2021/08/07
 */

#ifndef __RE_RENDER_H__
#define __RE_RENDER_H__

#include "config.h"
#include "buf.h"

void editorDrawRows(struct abuf *ab);
void editorDrawStatusBar(struct abuf *ab);
void editorDrawMessageBar(struct abuf *ab);

#endif // __RE_RENDER_H__

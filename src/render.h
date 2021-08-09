/**
 * 渲染相关逻辑
 *
 * Author: In0vati0n
 * Date: 2021/08/07
 */

#ifndef __RE_RENDER_H__
#define __RE_RENDER_H__

#include "common.h"
#include "config.h"
#include "buf.h"

/**
 * 根据内容绘制每一行
 */
void editorDrawRows(struct abuf *ab);

/**
 * 绘制状态栏
 */
void editorDrawStatusBar(struct abuf *ab);

/**
 * 绘制消息栏
 */
void editorDrawMessageBar(struct abuf *ab);

#endif // __RE_RENDER_H__

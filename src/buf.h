/**
 * 动态字符串相关
 *
 * Author: In0vati0n
 * Date: 2021/08/07
 */

#ifndef __RE_BUF_H__
#define __RE_BUF_H__

/**
 * 动态字符串 buff
 */
struct abuf
{
    char *b;
    int len;
};

#define ABUF_INIT {NULL, 0}

void abAppend(struct abuf *ab,
              const char *s, int len);

void abFree(struct abuf *ab);

#endif // __RE_BUF_H__

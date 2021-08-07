/**
 * 动态字符串相关
 *
 * Author: In0vati0n
 * Date: 2021/08/07
 */

#include "buf.h"

#include <stdlib.h>
#include <string.h>

void abAppend(struct abuf *ab,
              const char *s, int len)
{
    char *new = realloc(ab->b, ab->len + len);

    if (new == NULL) return;
    memcpy(&new[ab->len], s, len);
    ab->b = new;
    ab->len += len;
}

void abFree(struct abuf *ab)
{
    free(ab->b);
}

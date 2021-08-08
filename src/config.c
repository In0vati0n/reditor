/**
 * 配置相关
 *
 * Author: In0vati0n
 * Date: 2021/08/07
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>

struct editorConfig E;

void editorUpdateSyntax(erow *row);

/**
 * 将数据列转换成渲染列
 */
int editorRowCxToRx(erow *row, int cx)
{
    int rx = 0;
    int j;
    for (j = 0; j < cx; j++)
    {
        if (row->chars[j] == '\t')
            rx += (RE_TAB_STOP - 1) - (rx % RE_TAB_STOP);
        rx++;
    }

    return rx;
}

/**
 * 将渲染列转换成数据列
 */
int editorRowRxToCx(erow *row, int rx)
{
    int cur_rx = 0;
    int cx;
    for (cx = 0; cx < row->size; cx++)
    {
        if (row->chars[cx] == '\t')
            cur_rx += (RE_TAB_STOP - 1)
                - (cur_rx % RE_TAB_STOP);
        cur_rx++;

        if (cur_rx > rx) return cx;
    }

    return cx;
}

/**
 * 根据数据更新渲染值
 * 将其中 nonprintable 字符替换成可读字符
 */
void editorUpdateRow(erow *row)
{
    int tabs = 0;
    int j;
    for (j = 0; j < row->size; j++)
        if (row->chars[j] == '\t') tabs++;

    free(row->render);
    row->render = malloc(row->size + tabs * (RE_TAB_STOP - 1) + 1);

    int idx = 0;
    for (j = 0; j < row->size; j++)
    {
        if (row->chars[j] == '\t')
        {
            row->render[idx++] = ' ';
            while (idx % RE_TAB_STOP != 0) row->render[idx++] = ' ';
        }
        else
        {
            row->render[idx++] = row->chars[j];
        }
    }

    row->render[idx] = '\0';
    row->rsize = idx;

    editorUpdateSyntax(row);
}

/**
 * 向行数据中插入一个字符
 */
void editorRowInsertChar(erow *row, int at, int c)
{
    if (at < 0 || at > row->size) at = row->size;
    row->chars = realloc(row->chars, row->size + 2);
    memmove(&row->chars[at + 1],
            &row->chars[at],
            row->size - at + 1);
    row->size++;
    row->chars[at] = c;
    editorUpdateRow(row);
    E.dirty++;
}

/**
 * 在一行拼接插入一个字符串
 */
void editorRowAppendString(erow *row, char *s, size_t len)
{
    row->chars = realloc(row->chars, row->size + len + 1);
    memcpy(&row->chars[row->size], s, len);
    row->size += len;
    row->chars[row->size] = '\0';
    editorUpdateRow(row);
    E.dirty++;
}

/**
 * 从数据行中删除一个字符
 */
void editorRowDelChar(erow *row, int at)
{
    if (at < 0 || at >= row->size) return;
    memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
    row->size--;
    editorUpdateRow(row);
    E.dirty++;
}

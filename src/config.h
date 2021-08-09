/**
 * 配置相关
 *
 * Author: In0vati0n
 * Date: 2021/08/07
 */

#ifndef __RE_CONFIG__
#define __RE_CONFIG__

#include <time.h>
#include <termios.h>

#define RE_VERSION "0.0.2"
#define RE_TAB_STOP 4
#define RE_QUIT_TIMES 2

enum editorHighlight
{
    HL_NORMAL = 0,
    HL_COMMENT,
    HL_MLCOMMENT,
    HL_KEYWORD1,
    HL_KEYWORD2,
    HL_STRING,
    HL_NUMBER,
    HL_MATCH
};

#define HL_HIGHLIGHT_NUMBERS (1<<0)
#define HL_HIGHLIGHT_STRINGS (1<<1)

/**
 * 语法高亮配置
 */
struct editorSyntax
{
    char *filetype;
    char **filematch;
    char **keywords;
    char *singleline_comment_start;
    char *multiline_comment_start;
    char *multiline_comment_end;
    int flags;
};

/**
 * 行数据
 * editor row
 */
typedef struct erow
{
    int idx;
    int size;
    int rsize; // render size
    char *chars;
    char *render;
    unsigned char *hl; // highlight
    int hl_open_comment;
} erow;

/**
 * Editor 配置文件
 */
struct editorConfig
{
    /**
     * 当前光标在字符串(chars)中的位置
     */
    int cx, cy;

    /**
     * 当前光标在渲染(render)中的位置
     */
    int rx;

    /**
     * 屏幕行数
     */
    int screenrows;
    /**
     * 屏幕列数
     */
    int screencols;

    /**
     * 行偏移
     */
    int rowoff;
    /**
     * 列偏移
     */
    int coloff;

    /**
     * 数据行数
     */
    int numrows;

    /**
     * 行数据数组
     */
    erow *row;

    /**
     * 当前文件是否被修改
     */
    int dirty;

    /**
     * 文件名
     */
    char *filename;

    /**
     * 状态栏消息及时间戳
     */
    char statusmsg[80];
    time_t statusmsg_time;

    /**
     * 高亮配置
     */
    struct editorSyntax *syntax;
    
    /**
     * 终端原始配置
     */
    struct termios orig_termios;
};

/**
 * Editor 全局配置
 */
extern struct editorConfig E;

/**
 * 将数据列转换成渲染列
 */
int editorRowCxToRx(erow *row, int cx);

/**
 * 将渲染列转换成数据列
 */
int editorRowRxToCx(erow *row, int rx);

/**
 * 根据数据更新渲染值
 * 将其中 nonprintable 字符替换成可读字符
 */
void editorUpdateRow(erow *row);

/**
 * 向行数据中插入一个字符
 */
void editorRowInsertChar(erow *row, int at, int c);

/**
 * 在一行拼接插入一个字符串
 */
void editorRowAppendString(erow *row, char *s, size_t len);

/**
 * 从数据行中删除一个字符
 */
void editorRowDelChar(erow *row, int at);

#endif // __RE_CONFIG__

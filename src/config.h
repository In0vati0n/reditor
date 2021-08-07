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

#endif // __RE_CONFIG__

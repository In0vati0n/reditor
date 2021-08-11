/**
 * REditor 主程序
 *
 * Author: In0vati0n
 * Date: 2021/08/07
 */

/*** includes ***/

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>

/*** lua include ***/

#include "lua.h"

/*** rlib include ***/

#include "rlib.h"

/*** other include ***/

#include "common.h"
#include "input.h"
#include "config.h"
#include "buf.h"
#include "render.h"

#include "terminal_render.h"

/*** filetypes ***/

char *C_HL_extensions[] = {".c", ".h", ".cpp", NULL};

char *C_HL_keywords[] = {
    "switch", "if", "while", "for", "break", "continue",
    "return", "else", "struct", "union", "typedef",
    "static", "enum", "class", "case", "int|", "long|",
    "double|", "float|", "char|", "unsigned|", "signed|",
    "void|", NULL};

struct editorSyntax HLDB[] = {
    {"c",
     C_HL_extensions,
     C_HL_keywords,
     "//", "/*", "*/",
     HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS},
};

#define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))

/*** prototypes ***/

void editorSetStatusMessage(const char *fmt, ...);
void editorRefreshScreen();
char *editorPrompt(char *prompt, void (*callback)(char *, int));
int callLua(const char *func_name);

/*** terminal ***/

/**
 * 还原终端配置
 */
void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
        die("tcsetattr");
}

/**
 * 启动终端 raw mode
 * 
 * 终端默认为 canonical mode（cooked mode）
 * 此模式下进行输入需要按回车键确认后发送给程序
 * 因此将终端设置为 raw mode
 * 这样在终端输入就不需要按回车键确认
 * 通过 tcsetattr 设置终端特性到达目的
 */
void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1)
        die("tcgetattr");

    struct termios raw = E.orig_termios;

    // 读取终端属性
    tcgetattr(STDIN_FILENO, &raw);

    /**
     * input flag：
     * - BRKINT: 开启后，break condition 会产生一条 SIGINT 信号发送给程序，比如 ctrl-c
     *           https://www.cmrr.umn.edu/~strupp/serial.html#2_3_3
     * - CRNL: CR -> carriage return，NL -> new line
     *         修复 ctrl-m
     * - INPCK: 该特性启动奇偶校验，现代终端一般不会启用
     * - ISTRIP: 该特性将每次输入字节的第 8 位剥离，即设置为 0，一般也被关闭
     * - IXON: 特性会将一些特殊输入转换成 software flow control
     *         https://zh.wikipedia.org/wiki/%E8%BD%AF%E4%BB%B6%E6%B5%81%E6%8E%A7%E5%88%B6
     *         修复 ctrl-s、ctrl-q
     * 
     * output flag:
     * - OPOST: POST -> post-processing of output，即关闭输出后处理，直接输出原始内容
     * 
     * local flag：
     * - ECHO: 特性会将每一个按键打印到终端，关掉后不会再打印输入内容
     * - ICANON: 特性会按照一行行读取终端输入，关闭后改为按每一个 byte 读取
     * - IEXTEN: 特性在接收到特殊输入时等待新的输入，修复 ctrl-v、ctrl-o
     * - ISIG: 特性会将一些特殊输入转换成特定信号，比如：ctrl-c 会发送 SIGINT 信号，
     *       ctrl-z 发送 SIGTSTP 信号，修复 ctrl-c、ctrl-z、ctrl-y
     * 
     * - c_lflag: local flags
     * - c_iflag: input flags
     * - c_oflag: output flags
     * - c_cflag: control flags
     */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag &= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    /**
     * - VMIN: 设置 read() return 前至少需要从 input 流中读取多少 bytes
     *         这里设置为 0，即 read() 不需要读到任何输入便可以返回
     * - VTIME: 设置 read() 最大等待时间，单位是 1/10s，如果 read() 超时，则返回 0
     * 
     * 通过这样的设置避免 read() 阻塞整个程序运行
     */
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    // 设置终端属性
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

/*** syntax highlighting ***/

int is_separator(int c)
{
    return isspace(c) || c == '\0' || strchr(",.()+-/*=~%<>[];", c) != NULL;
}

void editorUpdateSyntax(erow *row)
{
    row->hl = realloc(row->hl, row->rsize);
    memset(row->hl, HL_NORMAL, row->rsize);

    if (E.syntax == NULL)
        return;

    char **keywords = E.syntax->keywords;

    char *scs = E.syntax->singleline_comment_start;
    char *mcs = E.syntax->multiline_comment_start;
    char *mce = E.syntax->multiline_comment_end;

    int scs_len = scs ? strlen(scs) : 0;
    int mcs_len = mcs ? strlen(mcs) : 0;
    int mce_len = mce ? strlen(mce) : 0;

    int prev_sep = 1;
    int in_string = 0;
    int in_comment = (row->idx > 0 && E.row[row->idx - 1].hl_open_comment);

    int i = 0;
    while (i < row->rsize)
    {
        char c = row->render[i];
        unsigned char prev_hl = (i > 0)
                                    ? row->hl[i - 1]
                                    : HL_NORMAL;

        if (scs_len && !in_string && !in_comment)
        {
            if (!strncmp(&row->render[i], scs, scs_len))
            {
                memset(&row->hl[i], HL_COMMENT,
                       row->rsize - i);
                break;
            }
        }

        if (mcs_len && mce_len && !in_string)
        {
            if (in_comment)
            {
                row->hl[i] = HL_MLCOMMENT;
                if (!strncmp(&row->render[i],
                             mce, mce_len))
                {
                    memset(&row->hl[i], HL_MLCOMMENT, mce_len);
                    i += mce_len;
                    in_comment = 0;
                    prev_sep = 1;
                    continue;
                }
                else
                {
                    i++;
                    continue;
                }
            }
            else if (!strncmp(&row->render[i], mcs, mcs_len))
            {
                memset(&row->hl[i], HL_MLCOMMENT, mcs_len);
                i += mcs_len;
                in_comment = 1;
                continue;
            }
        }

        if (E.syntax->flags & HL_HIGHLIGHT_STRINGS)
        {
            if (in_string)
            {
                row->hl[i] = HL_STRING;
                if (c == '\\' && i + 1 < row->rsize)
                {
                    row->hl[i + 1] = HL_STRING;
                    i += 2;
                    continue;
                }
                if (c == in_string)
                    in_string = 0;
                i++;
                prev_sep = 1;
                continue;
            }
            else
            {
                if (c == '"' || c == '\'')
                {
                    in_string = c;
                    row->hl[i] = HL_STRING;
                    i++;
                    continue;
                }
            }
        }

        if (E.syntax->flags & HL_HIGHLIGHT_NUMBERS)
        {
            if ((isdigit(c) && (prev_sep || prev_hl == HL_NUMBER)) || (c == '.' && prev_hl == HL_NUMBER))
            {
                row->hl[i] = HL_NUMBER;
                i++;
                prev_sep = 0;
                continue;
            }
        }

        if (prev_sep)
        {
            int j;
            for (j = 0; keywords[j]; j++)
            {
                int klen = strlen(keywords[j]);
                int kw2 = keywords[j][klen - 1] == '|';
                if (kw2)
                    klen--;

                if (!strncmp(&row->render[i],
                             keywords[j],
                             klen) &&
                    is_separator(row->render[i + klen]))
                {
                    memset(&row->hl[i],
                           kw2 ? HL_KEYWORD2 : HL_KEYWORD1,
                           klen);
                    i += klen;
                    break;
                }
            }

            if (keywords[j] != NULL)
            {
                prev_sep = 0;
                continue;
            }
        }

        prev_sep = is_separator(c);
        i++;
    }

    int changed = (row->hl_open_comment != in_comment);
    row->hl_open_comment = in_comment;
    if (changed && row->idx + 1 < E.numrows)
        editorUpdateSyntax(&E.row[row->idx + 1]);
}

int editorSyntaxToColor(int hl)
{
    switch (hl)
    {
    case HL_COMMENT:
    case HL_MLCOMMENT:
        return 36;
    case HL_KEYWORD1:
        return 33;
    case HL_KEYWORD2:
        return 32;
    case HL_STRING:
        return 35;
    case HL_NUMBER:
        return 31;
    case HL_MATCH:
        return 34;
    default:
        return 37;
    }
}

void editorSelectSyntaxHighlight()
{
    E.syntax = NULL;
    if (E.filename == NULL)
        return;

    char *ext = strrchr(E.filename, '.');

    for (unsigned int j = 0; j < HLDB_ENTRIES; j++)
    {
        struct editorSyntax *s = &HLDB[j];
        unsigned int i = 0;
        while (s->filematch[i])
        {
            int is_ext = (s->filematch[i][0] == '.');
            if ((is_ext && ext && !strcmp(ext, s->filematch[i])) || (!is_ext && strstr(E.filename, s->filematch[i])))
            {
                E.syntax = s;

                int filerow;
                for (filerow = 0; filerow < E.numrows; filerow++)
                {
                    editorUpdateSyntax(&E.row[filerow]);
                }

                return;
            }

            i++;
        }
    }
}

/*** editor operations ***/

/**
 * 向编辑器数据中添加一行
 */
void editorInsertRow(int at, char *s, size_t len)
{
    if (at < 0 || at > E.numrows)
        return;

    E.row = realloc(E.row, sizeof(erow) * (E.numrows + 1));
    memmove(&E.row[at + 1], &E.row[at],
            sizeof(erow) * (E.numrows - at));
    for (int j = at + 1; j <= E.numrows; j++)
        E.row[j].idx++;

    E.row[at].idx = at;

    E.row[at].size = len;
    E.row[at].chars = malloc(len + 1);
    memcpy(E.row[at].chars, s, len);
    E.row[at].chars[len] = '\0';

    E.row[at].rsize = 0;
    E.row[at].render = NULL;
    E.row[at].hl = NULL;
    E.row[at].hl_open_comment = 0;
    editorUpdateRow(&E.row[at]);

    E.numrows++;
    E.dirty++;
}

/**
 * 释放一行内存
 */
void editorFreeRow(erow *row)
{
    free(row->render);
    free(row->chars);
    free(row->hl);
}

/**
 * 删除一行
 */
void editorDelRow(int at)
{
    if (at < 0 || at > E.numrows)
        return;
    editorFreeRow(&E.row[at]);
    memmove(&E.row[at], &E.row[at + 1], sizeof(erow) * (E.numrows - at - 1));
    for (int j = at; j < E.numrows - 1; j++)
        E.row[j].idx--;
    E.numrows--;
    E.dirty++;
}

/**
 * 向编辑器中插入一个字符
 */
void editorInsertChar(int c)
{
    if (E.cy == E.numrows)
    {
        editorInsertRow(E.numrows, "", 0);
    }
    editorRowInsertChar(&E.row[E.cy], E.cx, c);
    E.cx++;
}

/**
 * 向编辑器中插入新行
 */
void editorInsertNewline()
{
    if (E.cx == 0)
    {
        editorInsertRow(E.cy, "", 0);
    }
    else
    {
        erow *row = &E.row[E.cy];
        editorInsertRow(E.cy + 1,
                        &row->chars[E.cx],
                        row->size - E.cx);
        row = &E.row[E.cy];
        row->size = E.cx;
        row->chars[row->size] = '\0';
        editorUpdateRow(row);
    }

    E.cy++;
    E.cx = 0;
}

/**
 * 编辑器删除一个字符
 */
void editorDelChar()
{
    if (E.cy == E.numrows)
        return;
    if (E.cx == 0 && E.cy == 0)
        return;

    erow *row = &E.row[E.cy];
    if (E.cx > 0)
    {
        editorRowDelChar(row, E.cx - 1);
        E.cx--;
    }
    else
    {
        E.cx = E.row[E.cy - 1].size;
        editorRowAppendString(&E.row[E.cy - 1],
                              row->chars,
                              row->size);
        editorDelRow(E.cy);
        E.cy--;
    }
}

/*** file i/o ***/

/**
 * 将行数据转换成一个字符串
 */
char *editorRowsToString(/* out */ int *buflen)
{
    int totlen = 0;
    int j;
    for (j = 0; j < E.numrows; j++)
        totlen += E.row[j].size + 1;
    *buflen = totlen;

    char *buf = malloc(totlen);
    char *p = buf;
    for (j = 0; j < E.numrows; j++)
    {
        memcpy(p, E.row[j].chars, E.row[j].size);
        p += E.row[j].size;
        *p = '\n';
        p++;
    }

    return buf;
}

/**
 * 打开文件
 */
void editorOpen(char *filename)
{
    free(E.filename);
    E.filename = strdup(filename);

    editorSelectSyntaxHighlight();

    FILE *fp = fopen(filename, "r");
    if (!fp)
        die("fopen");

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fp)) != -1)
    {
        while (linelen > 0 && (line[linelen - 1] == '\n' ||
                               line[linelen - 1] == '\r'))
        {
            linelen--;
        }

        editorInsertRow(E.numrows, line, linelen);
    }

    free(line);
    fclose(fp);
    E.dirty = 0;
}

void editorSave()
{
    if (E.filename == NULL)
    {
        E.filename = editorPrompt("Save as: %s", NULL);
        if (E.filename == NULL)
        {
            editorSetStatusMessage("Save aborted");
            return;
        }
        editorSelectSyntaxHighlight();
    }

    int len;
    char *buf = editorRowsToString(&len);

    // TODO 先将内容写到一个临时文件上
    // 写好后在覆盖原始文件，这样更安全

    int fd = open(E.filename, O_RDWR | O_CREAT, 0644);
    if (fd != -1)
    {
        if (ftruncate(fd, len) != -1)
        {
            if (write(fd, buf, len) == len)
            {
                close(fd);
                free(buf);
                E.dirty = 0;
                editorSetStatusMessage("%d bytes written to disk", len);
                return;
            }
        }

        close(fd);
    }

    free(buf);
    editorSetStatusMessage("Can't save! I/O error: %s", strerror(errno));
}

/*** find ***/

void editorFindCallback(char *query, int key)
{
    static int last_match = -1;
    static int direction = 1;

    static int saved_hl_line;
    static char *saved_hl = NULL;

    if (saved_hl)
    {
        memcpy(E.row[saved_hl_line].hl,
               saved_hl, E.row[saved_hl_line].rsize);
        free(saved_hl);
        saved_hl = NULL;
    }

    if (key == '\r' || key == '\x1b')
    {
        last_match = -1;
        direction = 1;
        return;
    }
    else if (key == ARROW_RIGHT || key == ARROW_DOWN)
    {
        direction = 1;
    }
    else if (key == ARROW_LEFT || key == ARROW_RIGHT)
    {
        direction = -1;
    }
    else
    {
        last_match = -1;
        direction = 1;
    }

    if (last_match == -1)
        direction = 1;
    int current = last_match;
    int i;
    for (i = 0; i < E.numrows; i++)
    {
        current += direction;
        if (current == -1)
            current = E.numrows - 1;
        else if (current == E.numrows)
            current = 0;

        erow *row = &E.row[current];
        char *match = strstr(row->render, query);
        if (match)
        {
            last_match = current;
            E.cy = current;
            E.cx = editorRowRxToCx(row,
                                   match - row->render);
            E.rowoff = E.numrows;

            saved_hl_line = current;
            saved_hl = malloc(row->rsize);
            memcpy(saved_hl, row->hl, row->rsize);
            memset(&row->hl[match - row->render],
                   HL_MATCH, strlen(query));
            break;
        }
    }
}

/**
 * 查找
 */
void editorFind()
{
    int saved_cx = E.cx;
    int saved_cy = E.cy;
    int saved_coloff = E.coloff;
    int saved_rowoff = E.rowoff;

    char *query = editorPrompt("Search: %s"
                               " (Use ESC/Arrows/Enter)",
                               editorFindCallback);
    if (query)
    {
        free(query);
    }
    else
    {
        // 如果用户取消，则还原光标位置
        E.cx = saved_cx;
        E.cy = saved_cy;
        E.coloff = saved_coloff;
        E.rowoff = saved_rowoff;
    }
}

/*** output ***/

/**
 * 屏幕滚动
 */
void editorScroll()
{
    E.rx = 0;
    if (E.cy < E.numrows)
        E.rx = editorRowCxToRx(&E.row[E.cy], E.cx);

    if (E.cy < E.rowoff)
        E.rowoff = E.cy;

    if (E.cy >= E.rowoff + E.screenrows)
        E.rowoff = E.cy - E.screenrows + 1;

    if (E.rx < E.coloff)
        E.coloff = E.rx;

    if (E.rx >= E.coloff + E.screencols)
        E.coloff = E.rx - E.screencols + 1;
}

/**
 * 刷新屏幕
 */
void editorRefreshScreen()
{
    editorScroll();

    struct abuf ab = ABUF_INIT;

    /**
     * ?25l：隐藏光标
     * 参看：http://vt100.net/docs/vt100-ug/chapter3.html#SM
     */
    abAppend(&ab, "\x1b[?25l", 6);

    /**
     * 4 bytes to write
     * - \x1b: escape character 转义字符（也可以写成数字 27)
     * - [: <esc>[ 组成转义序列，表示后面是一个转义命令
     * - 2: J 命令参数，表示清空屏幕，转义序列参数在命令前面
     * - J: 转义命令（erase in display），清空屏幕
     *
     * 整个命令为 <ecs>[2J
     * J 命令详细参考：https://vt100.net/docs/vt100-ug/chapter3.html#ED
     *
     * 这里使用的是 VT100 转义序列
     * 参看：https://en.wikipedia.org/wiki/VT100
     *
     * 要让程序更加健壮可以使用 ncurses 库
     * 参看：https://en.wikipedia.org/wiki/Ncurses
     */
    // 优化：清理每行代替清理全屏
    // abAppend(&ab, "\x1b[2J", 4);

    /**
     * <ecs>[H：cursor position 命令
     * 参看：https://vt100.net/docs/vt100-ug/chapter3.html#CUP
     */
    abAppend(&ab, "\x1b[H", 3);

    // 绘制每行
    editorDrawRows(&ab);
    // 绘制状态栏
    editorDrawStatusBar(&ab);
    // 绘制消息栏
    editorDrawMessageBar(&ab);

    // 设置指针位置
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH",
             (E.cy - E.rowoff) + 1,
             (E.rx - E.coloff) + 1);
    abAppend(&ab, buf, strlen(buf));

    /**
     * ?25h：显示光标
     * 参看：http://vt100.net/docs/vt100-ug/chapter3.html#RM
     */
    abAppend(&ab, "\x1b[?25h", 6);

    // 将缓冲区内容写入终端
    write(STDOUT_FILENO, ab.b, ab.len);

    abFree(&ab);
}

/**
 * 编辑器设置状态信息
 */
void editorSetStatusMessage(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(E.statusmsg, sizeof(E.statusmsg), fmt, ap);
    va_end(ap);
    E.statusmsg_time = time(NULL);
}

/*** input ***/

/**
 * 编辑器弹窗
 */
char *editorPrompt(char *prompt, void (*callback)(char *, int))
{
    size_t bufsize = 128;
    char *buf = malloc(bufsize);

    size_t buflen = 0;
    buf[0] = '\0';

    while (1)
    {
        editorSetStatusMessage(prompt, buf);
        editorRefreshScreen();

        int c = ti_editorReadKey();
        if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE)
        {
            if (buflen != 0)
                buf[--buflen] = '\0';
        }
        else if (c == '\x1b')
        {
            editorSetStatusMessage("");
            if (callback)
                callback(buf, c);
            free(buf);
            return NULL;
        }
        else if (c == '\r')
        {
            if (buflen != 0)
            {
                editorSetStatusMessage("");
                if (callback)
                    callback(buf, c);
                return buf;
            }
        }
        else if (!iscntrl(c) && c < 128)
        {
            if (buflen == bufsize - 1)
            {
                bufsize *= 2;
                buf = realloc(buf, bufsize);
            }

            buf[buflen++] = c;
            buf[buflen] = '\0';
        }

        if (callback)
            callback(buf, c);
    }
}

/**
 * 处理光标移动
 */
void editorMoveCursor(int key)
{
    erow *row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];

    switch (key)
    {
    case ARROW_LEFT:
        if (E.cx != 0)
        {
            E.cx--;
        }
        else if (E.cy > 0)
        {
            E.cy--;
            E.cx = E.row[E.cy].size;
        }
        break;

    case ARROW_RIGHT:
        if (row && E.cx < row->size)
        {
            E.cx++;
        }
        else
        {
            if (E.cy + 1 < E.numrows)
            {
                E.cy++;
                E.cx = 0;
            }
        }
        break;

    case ARROW_UP:
        if (E.cy != 0)
            E.cy--;
        break;

    case ARROW_DOWN:
        if (E.cy < E.numrows)
            E.cy++;
        break;
    }

    // 当光标上下移动时，将光标限制在当前行长度内
    row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
    int rowlen = row ? row->size : 0;
    if (E.cx > rowlen)
        E.cx = rowlen;
}

/**
 * 编辑器输入处理
 */
void editorProcessKeypress()
{
    static int quit_times = RE_QUIT_TIMES;

    int c = ti_editorReadKey();

    switch (c)
    {
    case '\r':
        editorInsertNewline();
        break;

    case CTRL_KEY('q'):
    {
        if (E.dirty && quit_times > 0)
        {
            editorSetStatusMessage("WARNING!!! File has unsaved changes. "
                                   "Press Ctrl-Q %d more times to quit.",
                                   quit_times);
            quit_times--;
            return;
        }

        // 程序退出时清理屏幕
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);

        exit(0);
        break;
    }

    case CTRL_KEY('s'):
        editorSave();
        break;

    case HOME_KEY:
        E.cx = 0;
        break;

    case END_KEY:
        if (E.cy < E.numrows)
            E.cx = E.row[E.cy].size;
        break;

    case CTRL_KEY('f'):
        editorFind();
        break;

    case BACKSPACE:
    case CTRL_KEY('h'):
    case DEL_KEY:
        if (c == DEL_KEY)
            editorMoveCursor(ARROW_RIGHT);
        editorDelChar();
        break;

    case PAGE_UP:
    case PAGE_DOWN:
    {
        if (c == PAGE_UP)
        {
            E.cy = E.rowoff;
        }
        else if (c == PAGE_DOWN)
        {
            E.cy = E.rowoff + E.screenrows - 1;
            if (E.cy > E.numrows)
                E.cy = E.numrows;
        }

        int times = E.screenrows;
        while (times--)
            editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
        break;
    }

    case ARROW_UP:
    case ARROW_LEFT:
    case ARROW_DOWN:
    case ARROW_RIGHT:
        editorMoveCursor(c);
        break;

    case CTRL_KEY('l'):
    case '\x1b':
        break;

    default:
        editorInsertChar(c);
        break;
    }

    quit_times = RE_QUIT_TIMES;
}

/*** init ***/

/**
 * 初始化编辑器
 */
void initEditor()
{
    E.cx = 0;
    E.cy = 0;
    E.rx = 0;
    E.rowoff = 0;
    E.coloff = 0;
    E.numrows = 0;
    E.row = NULL;
    E.dirty = 0;
    E.filename = NULL;
    E.statusmsg[0] = '\0';
    E.statusmsg_time = 0;
    E.syntax = NULL;

    // 得到终端尺寸
    if (tr_getWindowSize(&E.screenrows, &E.screencols) == -1)
        die("tr_getWindowSize");

    // 一行留给状态栏
    // 一行留给消息栏
    E.screenrows -= 2;
}

/*** lua ***/

static lua_State *L;

int lua_getWorkDir(lua_State *L)
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        lua_pushstring(L, cwd);
        return 1;
    }
    else
    {
        die("getcwd");
    }

    return 0;
}

int lua_editorRefreshScreen(lua_State *L)
{
    editorRefreshScreen();
    return 0;
}

int lua_editorProcessKeypress(lua_State *L)
{
    editorProcessKeypress();
    return 0;
}

int lua_die(lua_State *L)
{
    die(lua_tostring(L, 1));
    return 0;
}

int lua_exit(lua_State *L)
{
    // 程序退出时清理屏幕
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    exit(lua_tonumber(L, 1));
    return 0;
}

/**
 * 初始化 Lua
 */
int initLua(int argc, char *argv[])
{
    L = luaL_newstate();

    luaL_openlibs(L);

    lua_newtable(L);
    lua_setglobal(L, RE_LUA_GLOBAL_NAME);

    lua_getglobal(L, RE_LUA_GLOBAL_NAME);

    // 设置各种辅助函数
    lua_pushcfunction(L, lua_getWorkDir);
    lua_setfield(L, -2, "getWorkDir");

    lua_pushcfunction(L, lua_editorRefreshScreen);
    lua_setfield(L, -2, "refreshScreen");

    lua_pushcfunction(L, lua_editorProcessKeypress);
    lua_setfield(L, -2, "processKeypress");

    lua_pushcfunction(L, lua_die);
    lua_setfield(L, -2, "die");

    lua_pushcfunction(L, lua_exit);
    lua_setfield(L, -2, "exit");

    // 设置 argv 参数
    lua_newtable(L);
    for (int i = 0; i < argc; i++)
    {
        lua_pushstring(L, argv[i]);
        lua_seti(L, -2, i);
    }
    lua_setfield(L, -2, "argv");

    lua_pop(L, 1); // pop reditor table

    // 初始化 input 库
    ti_initLib(L);

    // 初始化终端渲染库
    tr_initLib(L);

    return 0;
}

int traceback(lua_State *L)
{
    lua_getglobal(L, "debug");
    lua_getfield(L, -1, "traceback");
    lua_pushvalue(L, 1);
    lua_pushinteger(L, 2);
    lua_call(L, 2, 1);
    return 1;
}

int callLua(const char *func_name)
{
    lua_pushcfunction(L, traceback);
    lua_getglobal(L, func_name);

    int ret = lua_pcall(L, 0, 0, lua_gettop(L) - 1);
    if (ret != 0)
    {
        const char *error = lua_tostring(L, -1);

        RLOG_ERROR("Call lua error: %s", error);

        lua_getglobal(L, "log_error");
        lua_pushstring(L, error);
        lua_pcall(L, 1, 0, 0);
    }

    return ret;
}

int luaxResume(lua_State *L, int nargs, int *nres)
{
#if LUA_VERSION_NUM >= 504
    return lua_resume(L, NULL, nargs, nres);
#elif LUA_VERSION_NUM >= 502
    return lua_resume(L, NULL, nargs);
#else
    return lua_resume(L, nargs);
#endif
}

int luaMainLoop()
{
    // 加载 boot.lua，返回一个协程
    lua_getglobal(L, "require");
    lua_pushstring(L, "scripts.boot");
    lua_call(L, 1, 1);

    // 主循环
    lua_newthread(L);
    lua_pushvalue(L, -2); // boot return value
    int stackpos = lua_gettop(L);
    int nres;

    while (luaxResume(L, 0, &nres) == LUA_YIELD)
    {
#if LUA_VERSION_NUM >= 504
        lua_pop(L, nres);
#else
        lua_pop(L, lua_gettop(L) - stackpos);
#endif
    }

    if (lua_isnumber(L, -1))
        return (int)lua_tonumber(L, -1);

    return 0;
}

int main(int argc, char *argv[])
{
    // 设置退出还原
    atexit(disableRawMode);

    // 启动 Raw Mode
    enableRawMode();

    initEditor();

    // 初始化 lua
    initLua(argc, argv);

    // if (argc >= 2)
    //     editorOpen(argv[1]);

    editorSetStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");

    // 主循环
    int ret = luaMainLoop();

    lua_close(L);

    return ret;
}

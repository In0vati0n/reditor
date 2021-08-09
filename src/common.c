/**
 * 终端渲染相关逻辑
 *
 * Author: In0vati0n
 * Date: 2021/08/09
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "common.h"

/**
 * 终止程序运行，并输出错误信息
 */
void die(const char *s)
{
    // 程序退出时清理屏幕
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}

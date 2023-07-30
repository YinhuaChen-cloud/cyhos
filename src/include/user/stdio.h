#ifndef CYHOS_STDIO_H
#define CYHOS_STDIO_H

#include <stdarg.h>

#define STDOUT 0
#define STDIN  1
#define STDERR 2

// 主要提供的库函数
extern int printf(const char *format, ...);

extern void perror(const char *s);

extern int puts(const char *s);

extern char *readline(const char *prompt);

extern int sprintf(char *str, const char *format, ...);

// 通常用于辅助的函数，但也可以被用户程序使用
extern int vsprintf(char *str, const char *format, va_list ap);

#endif

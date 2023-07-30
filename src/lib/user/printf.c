#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define BUF_LEN 1024
// buf, 静态全局变量，用来存放将要被 printf 或者 sprintf 的字符串
// 使用 static 修饰，防止重名问题
static char buf[BUF_LEN];

// 主要提供的库函数
int printf(const char *format, ...)
{
    va_list args;
    int i;

    va_start(args, format);

    i = vsprintf(buf, format, args);

    va_end(args);

    write(STDOUT, buf, i);

    return i;
}

void perror(const char *s)
{
    write(STDERR, s, strlen(s));
    write(STDERR, "\n", 1);
}

int puts(const char *s) 
{
    int retval = write(STDOUT, s, strlen(s));
    if(retval < 0)
        return retval;
    return write(STDOUT, "\n", 1);
}

char *readline(const char *prompt)
{
    write(STDOUT, prompt, strlen(prompt));

    read(STDIN, buf, BUF_LEN);

    return (char *)buf;
}

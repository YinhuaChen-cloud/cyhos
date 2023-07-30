#ifndef CYHOS_STDARG_H
#define CYHOS_STDARG_H

// va_list: 表示可变参数列表，实际上就是一个指针
typedef char *va_list;

// TODO: 等待测试
// 使用方法：va_start(va_list 变量, 可变参数列表前面一个变量)  把指向参数列表的指针指向 NULL
#define va_start(ap, v) (ap = (va_list)&v + sizeof(char *))
// 使用方法：va_arg(va_list 变量, 类型)  把指向参数列表的指针指向 NULL
#define va_arg(ap, t) (*(t *)((ap += sizeof(char *)) - sizeof(char *)))
// 使用方法：va_end(va_list 变量)  把指向参数列表的指针指向 NULL
#define va_end(ap) (ap = (va_list)0)

#endif


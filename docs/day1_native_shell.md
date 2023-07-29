# Day 1 shell

<font color="red">今日目标：实现一个可以在 native 环境下运行的 shell 程序，支持数个命令</font>

<font color="orange">在今天的代码中会调用很多 Linux 环境下存在的库函数，在我们自己的实现的 cyhos 中，这些库函数将需要迁移或者我们自己实现</font>

我们制作操作系统的第一个 milestone 是拥有一个能够运行数个命令的shell

我们先实现一个 shell，再想办法用一个操作系统去支持它，这是一种自顶向下的开发方式

先实现一个能在 linux 环境中运行的 shell

## 分析

需要支持一些简单的 shell 命令：

- ls
- cd
- pwd
- mkdir
- rmdir
- rm
- cat
- clear
- exit

cyhsh.c 位于 src/builtin 文件夹中，说明这是一个 cyhos 操作系统内置的用户程序

其中，有些命令属于 shell 的内置命令，这里列出重要的：

- logo
- cd
- pwd
- exit

外部程序包括:

- ls
- mkdir
- rmdir
- rm
- cat
- clear

## 实现 Step 1 清屏

一个 shell 程序在启动时，应该能够清空整个屏幕

在 Linux shell 中，clear 属于外部程序，于是需要 shell 创建子进程去执行 clear，如下：

```c
    // 清空屏幕
    // Create a child process to clear screen
    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process
        // 注意：这里的 static 修饰似乎是必要的，否则子进程不会保留 "clear_argv" 所占的内存，这会导致子进程无法正确执行 clear 程序
        static char *clear_argv[] = {"clear"};
        execvp(clear_argv[0], clear_argv);
        // 只有在 execvp 失败的时候，才会返回到这里，否则不会返回
        perror("Execvp failed");
        exit(1);
    } else {
        // Parent process
        wait(NULL); // 等待子进程退出
    }
```

在 src/arch/native/crt0.c 中，使用下述代码作为启动代码

```c
extern int cyhsh_main();

int main() {
    cyhsh_main();
}
```

## 实现 Step 2 打印logo

使用 figlet 可以生成标题，我们使用这个软件生成 cyhos 的 logo，随后进行打印

```c
#include <string.h>

static const char *cyhos_logo[] = {
    "            _                    ",
    "  ___ _   _| |__   ___  ___      ",
    " / __| | | | '_ \\ / _ \\/ __|   ",
    "| (__| |_| | | | | (_) \\__ \\   ",
    " \\___|\\__, |_| |_|\\___/|___/  ",
    "      |___/                      ",
};

// 打印 logo
for(int i = 0; i < sizeof(cyhos_logo)/sizeof(cyhos_logo[0]); i++)
    puts(cyhos_logo[i]);
```

## 实现 Step3 实现相关 Makefile 进行链接

想要了解这部分，请 RTFSC


## 实现 Step4 尝试构建 native-vscode-gdb 调试开发环境




## 实现 Step5 打印 prompt 和 当前目录

实现 print_prompt
```c
void print_prompt()
{
    // 获取当前目录的路径
    getcwd(cwd, MAX_PATH_LEN);

    char *base = basename(cwd); 

    printf("[root %s]# ", base);
}
```

效果如下:

```bash
            _                    
  ___ _   _| |__   ___  ___      
 / __| | | | '_ \ / _ \/ __|   
| (__| |_| | | | | (_) \__ \   
 \___|\__, |_| |_|\___/|___/  
      |___/                      
[root cyhos]# %  
```

## 实现 Step6 获取用户命令并且解析

readline 是调用库函数，cmd_parse 则是自己实现的

其实有 getopt 这个库函数帮助我们解析命令，但这个函数实现的功能远超出我们的需求，因此使用我们自己实现的 cmd_parse

```c
// 读取用户输入，遇到回车时返回，不会携带回车
cmd = readline("");
// 输入为空，则重新开始循环，要求用户输入新的命令
if (cmd[0] == 0)
{
    continue;
}
// 解析命令, argc 是参数数量，argv 是参数数组，cmd 就是要解析的命令
// 解析完成后，储存命令的指针指向的字符串里的空格会被替换成 \0
int argc = cmd_parse(cmd, argv, ' ');
// 如果解析出的参数数量不在范围内，要求用户重新输入
if (argc < 0 || argc >= MAX_ARG_NR)
{
    continue;
}
```

## 实现 Step7 根据获取的 argc 和 argv 执行该命令

首先判断是否是内置命令，若不是，则fork一个新的子进程，然后使用 execvp 命令去执行它

```c
        // 如果解析命令没问题，则执行命令
        // 首先判断命令是内置命令还是外部程序
        // 若是内置命令，调用相应的函数
        // 我们一共支持 logo, cd, pwd, exit 四个内置命令
        int code_status = execute(argc, argv);
        if(0 == code_status)
        {
            continue;
        }
        // 若是外部程序，则需要创建子进程去执行它
        // 让子进程执行命令，父进程则用于维持 Shell
        // Create a child process
        pid_t pid = fork();

        if (pid < 0) {
            // Fork failed
            perror("Fork failed");
            exit(1);
        } else if (pid == 0) {
            // Child process
            execvp(argv[0], argv);
            // 只有在 execvp 失败的时候，才会返回到这里，否则不会返回
            perror("Execvp failed");
            exit(1);
        } else {
            // Parent process
            wait(NULL); // 等待子进程退出
        }
```






















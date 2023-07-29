#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <libgen.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

// 支持的最长命令长度
#define MAX_CMD_LEN 256
// 支持的最多参数 --- TODO:
#define MAX_ARG_NR 16
// 支持的最长路径 --- TODO:
#define MAX_PATH_LEN 1024
// 缓冲区大小 --- TODO:
#define BUFLEN 1024

// 存放当前目录路径
static char cwd[MAX_PATH_LEN];
// 存放用户输入的命令
static char *cmd;
// 用来存放命令解析后获得的参数的数组
static char *argv[MAX_ARG_NR];

static const char *cyhos_logo[] = {
    "            _                    ",
    "  ___ _   _| |__   ___  ___      ",
    " / __| | | | '_ \\ / _ \\/ __|   ",
    "| (__| |_| | | | | (_) \\__ \\   ",
    " \\___|\\__, |_| |_|\\___/|___/  ",
    "      |___/                      ",
};

void print_prompt()
{
    // 获取当前目录的路径
    getcwd(cwd, MAX_PATH_LEN);

    char *base = basename(cwd); 

    printf("[root %s]# ", base);
}

void builtin_logo()
{
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

    // 打印 logo
    for(int i = 0; i < sizeof(cyhos_logo)/sizeof(cyhos_logo[0]); i++)
        puts(cyhos_logo[i]);
}

void builtin_cd(int argc, char *argv[])
{
    // 用来改变工作目录的库函数
    chdir(argv[1]);
}

void builtin_pwd()
{
    // 用来获取当前工作目录的库函数
    getcwd(cwd, MAX_PATH_LEN);
    printf("%s\n", cwd);
}

void builtin_exit()
{
    exit(0);
}

// 这个函数根据 argc 和 argv 执行 shell 的内置命令
// 如果执行成功，那么返回 0
// 如果执行失败，尤其是当所要执行的命令不是内置命令时，则返回非零
static int execute(int argc, char *argv[])
{
    int retval = -1;

    char *line = argv[0];
    if (!strcmp(line, "logo"))
    {
        builtin_logo();
        retval = 0;
    }
    if (!strcmp(line, "cd"))
    {
        builtin_cd(argc, argv);
        retval = 0;
    }
    if (!strcmp(line, "pwd"))
    {
        builtin_pwd();
        retval = 0;
    }
    if (!strcmp(line, "exit"))
    {
        builtin_exit();
        retval = 0;
    }

    return retval;
}

static int cmd_parse(char *cmd, char *argv[], char token)
{
    assert(cmd != NULL);

    char *next = cmd;
    int argc = 0;
    while (*next && argc < MAX_ARG_NR)
    {
        while (*next == token)
        {
            next++;
        }
        if (*next == 0)
        {
            break;
        }
        argv[argc++] = next;
        while (*next && *next != token)
        {
            next++;
        }
        if (*next)
        {
            *next = 0;
            next++;
        }
    }
    argv[argc] = NULL;
    return argc;
}

// 这是shell程序的主循环
int cyhsh_main()
{
    // cwd 是用来存放当前目录路径的缓冲区，这里要对它清空
    memset(cwd, 0, sizeof(cwd));

    // 刚启动时，在窗口主界面打印 logo
    builtin_logo();

    // shell 程序在接受 exit 命令之前是一个死循环
    while (true)
    {
        // 打印 prompt
        print_prompt();
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
    }
    // error: should not reach here!
    return 0;
}

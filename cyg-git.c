#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <windows.h>

#define GIT_PATH "git.exe"
#define MAX_CMDLINE_PATH 4096
#define WEXITSTATUS(status) (((status) & 0xFF00) >> 8)  // 手动定义 WEXITSTATUS

// 函数：转义字符串中的引号
void escape_quotes(const char *src, char *dest) {
    while (*src) {
        if (*src == '"') {
            // 转义双引号
            *dest++ = '\\';
            *dest++ = '"';
        } else {
            *dest++ = *src;
        }
        src++;
    }
    *dest = '\0';  // 确保字符串结束
}

// 函数：构建命令行字符串
void build_command_line(char *commandLine, char *args[], int argc) {
    char escapedExePath[MAX_PATH];
    escape_quotes(args[0], escapedExePath);

    // 拼接可执行文件路径
    strcpy(commandLine, "\"");
    strcat(commandLine, escapedExePath);
    strcat(commandLine, "\"");

    if (argc > 1)  // 拼接每个参数
    for (int i = 1; i < argc; i++) {
        strcat(commandLine, " ");
        char escapedArg[MAX_CMDLINE_PATH];
        escape_quotes(args[i], escapedArg);
        strcat(commandLine, "\"");
        strcat(commandLine, escapedArg);
        strcat(commandLine, "\"");
    }
}

char* get_path(char*path, char mode){
    // basecmd_len + quote(1) + path_len + quote(1) + '\0'(1)
    int cmd_len = strlen("cygpath -_ ") + strlen(path) + 2 + 1;
    char *cmd = malloc(cmd_len);
    sprintf(cmd, "cygpath -%c \"%s\"", mode, path);

    FILE *cmd_fp = popen(cmd, "r");
    if (cmd_fp == NULL) {
        perror("get_path: popen failed");
        exit(EXIT_FAILURE);
    }

    char *mod_path = malloc(MAX_PATH + 1);
    fgets(mod_path, MAX_PATH + 1, cmd_fp); // 读取转换后的路径
    pclose(cmd_fp);

    int mod_path_len = strlen(mod_path);
    if (mod_path[mod_path_len - 1] == '\n'){
        mod_path[mod_path_len - 1] = '\0';
    }
    if (mod_path[mod_path_len - 2] == '\r'){
        mod_path[mod_path_len - 2] = '\0';
    }

    return mod_path;
}

int main(int argc, char *argv[]) {

    char wrap_output = 0;

    char* mod_args[argc + 1];  // 用于存储命令参数，最后还需要一个 NULL
    mod_args[0] = GIT_PATH;
    mod_args[argc] = NULL;

    // 处理输入参数
    for (int i = 1; i < argc; i++) {

        char *arg = argv[i];

        if (i == 1 && 0 == strcmp(arg, "rev-parse")){
            wrap_output = 1;
            mod_args[i] = arg;
            continue;
        }

        if (arg[0] != '-'){
            char *arg_mod = get_path(arg, 'u');
            mod_args[i] = arg_mod;
            continue;
        }

        mod_args[i] = arg;
    }

    char cmdline[MAX_CMDLINE_PATH];
    build_command_line(cmdline, mod_args, argc);
    if (wrap_output){
        FILE *fp = popen(cmdline, "r");
        if (fp == NULL) {
            perror("wrap_output: popen failed");
            exit(EXIT_FAILURE);
        }
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), fp) != NULL){
            if (buffer[0] == '-'){
                printf("%s", buffer);
            } else {
                char *mod_path = get_path(buffer, 'w');
                printf("%s", mod_path);
            }
        }
        int status = pclose(fp);
        return WEXITSTATUS(status);
    } else {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        // 创建子进程
        if (!CreateProcess(NULL, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            fprintf(stderr, "CreateProcess failed (%d)\n", GetLastError());
            return EXIT_FAILURE;
        }

        // 等待子进程结束
        WaitForSingleObject(pi.hProcess, INFINITE);

        // 获取子进程的退出码
        DWORD exit_code;
        GetExitCodeProcess(pi.hProcess, &exit_code);

        // 关闭进程和线程句柄
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return (int) exit_code;
    }

    return 0;
}

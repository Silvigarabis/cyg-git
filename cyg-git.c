#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char wrap_output = 0;
    char *args[argc + 1];  // 用于存储修改后的参数
    int i, j = 1;

    // 设置PATH环境变量
    setenv("PATH", "/usr/bin", 1);

    // 处理输入的参数
    for (i = 1; i < argc; i++) {
        if (argv[i] == NULL || strlen(argv[i]) == 0) {
            continue;  // 跳过空参数
        } else if (strcmp(argv[i], "rev-parse") == 0 && i == 1) {
            args[0] = argv[0];  // 保留程序名
            args[1] = argv[i];  // 将rev-parse设为第二个参数
            wrap_output = 1;
            j = 2;  // 下一个有效参数的位置
        } else if (argv[i][0] != '-') {
            char *cmd = malloc(strlen("cygpath -u ") + strlen(argv[i]) + 1);
            sprintf(cmd, "cygpath -u \"%s\"", argv[i]);
            FILE *fp = popen(cmd, "r");
            if (fp == NULL) {
                perror("popen failed");
                exit(EXIT_FAILURE);
            }
            fgets(cmd, strlen(argv[i]) + 1, fp); // 读取转换后的路径
            pclose(fp);
            args[j++] = cmd;
        } else {
            args[j++] = argv[i];
        }
    }
    args[j] = NULL;

    // 根据wrap_output决定如何处理输出
    if (wrap_output) {
        FILE *fp = popen("git rev-parse", "r");
        if (fp == NULL) {
            perror("popen failed");
            exit(EXIT_FAILURE);
        }
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            if (buffer[0] == '-') {
                printf("%s", buffer);
            } else {
                char *cmd = malloc(strlen("cygpath -w ") + strlen(buffer) + 1);
                sprintf(cmd, "cygpath -w \"%s\"", buffer);
                FILE *wp = popen(cmd, "r");
                if (wp == NULL) {
                    perror("popen failed");
                    exit(EXIT_FAILURE);
                }
                fgets(buffer, sizeof(buffer), wp);
                pclose(wp);
                printf("%s", buffer);
                free(cmd);
            }
        }
        int status = pclose(fp);
        return WEXITSTATUS(status);
    } else {
        execvp("git", args);
        perror("exec failed");
        return EXIT_FAILURE;
    }
}
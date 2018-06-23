#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SIZE 1024
#define MAX_PIPE_NUM 25

char* buff;

void split(char** argv, char* argstr, char* splitStr)
{
    if(argstr == NULL) {
        argv[0] = NULL;
        return ;
    }
    char *tmp;
    int len = strlen(argstr);
    int argi = 0;
    tmp = strtok(argstr, splitStr);
    while(tmp) {
        int len = strlen(tmp);
        argv[argi] = (char*)malloc(len+1);
        strcpy(argv[argi], tmp);
        argi++;
        tmp = strtok(NULL, splitStr);
    }
    argv[argi] = NULL;
}

char* getCommand(char* buff)
{
    char* tmp = strtok(buff, " ");
    if(tmp)
        return tmp;

    return NULL;
}

void cd(char* path)
{
    chdir(path);
}

void pwd()
{
    char cwd[105];
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
}

void ls(char** argv)
{
    pid_t pid;
    pid = fork();
    if(pid == 0) {
        execvp("ls", argv);
        _exit(127);
    } else {
        wait(NULL);
    }
}

void cat(char** argv)
{
    pid_t pid;
    pid = fork();
    if(pid == 0) {
        execvp("cat", argv);
        _exit(127);
    } else {
        wait(NULL);
    }
}

void echo(char** argv)
{
    pid_t pid;
    pid = fork();
    if(pid == 0) {
        execvp("echo", argv);
        _exit(127);
    } else {
        wait(NULL);
    }
}

void oRedirect(char* path)
{
    if(!path)
        return ;

    int file;
    if((file = creat(path, 0664)) < 0)
        perror("Error when create file.");
    dup2(file, 1);
    close(file);

    return ;
}

void iRedirect(char* path)
{
    if(!path)
        return ;

    int file;
    if((file = open(path, O_RDONLY, 0664)) < 0)
        perror("Error when open file.");
    dup2(file, 0);
    close(file);

    return ;
}

void noRedirect()
{
    int file;
    file = open("/dev/tty", O_RDWR);
    dup2(file, 0);
    dup2(file, 1);
    dup2(file, 2);
}

int redirect(char** argv)
{
    int i = 0;
    int ret = 0;
    while(argv[i]) {
        if(!strcmp(argv[i], ">")) {
            oRedirect(argv[i+1]);
            argv[i] = NULL;
            ret = 1;
        } else if(!strcmp(argv[i], "<")) {
            iRedirect(argv[i+1]);
            argv[i] = NULL;
            ret = 1;
        }
        i++;
    }

    return ret;
}

void execCommand(char** argv)
{
    // I/O redirect
    int flag = redirect(argv);

    if(!strcmp(buff, "exit")) {
        exit(0);
    }
    if(!strcmp(argv[0], "cd")) {
        cd(buff+3);
    } else if(!strcmp(argv[0], "pwd")) {
        pwd();
    } else if(!strcmp(argv[0], "ls")) {
        ls(argv);
    } else if(!strcmp(argv[0], "echo")) {
        echo(argv);
    } else if(!strcmp(argv[0], "cat")) {
        cat(argv);
    } else {
        pid_t pid = fork();
        if(pid == 0) {
            execvp(argv[0], argv);
            _exit(127);
        } else {
            wait(NULL);
        }
    }

    // recovery redirect
    if(flag) noRedirect();
}

int main(int argc, char *argv[])
{
    buff = (char*)malloc(SIZE);

    int fd0 = dup(0);
    int fd1 = dup(1);

    while(1) {
        printf("$ ");
        fgets(buff, SIZE, stdin);
        buff[strlen(buff)-1] = '\0';

        char * commands[105];
        split(commands, buff, "|");

        int pfd[MAX_PIPE_NUM][2]; // 0 in 1 out

        int i = 0;
        while(commands[i]) {
            char* argv[25];
            split(argv, commands[i], " ");

            if(i == 0 && commands[i+1] == NULL) {
                // not use pipe
                execCommand(argv);
            } else if(i == 0) {
                pipe(pfd[i]);
                dup2(pfd[i][1], 1);
                close(pfd[i][1]);

                execCommand(argv);

                // recovery redirect
                dup2(fd1, 1);
            } else if(commands[i+1] == NULL) {
                dup2(pfd[i-1][0], 0);
                close(pfd[i-1][0]);

                execCommand(argv);

                // recovery redirect
                dup2(fd0, 0);
            } else {
                pipe(pfd[i]);
                dup2(pfd[i][1], 1);
                close(pfd[i][1]);
                dup2(pfd[i-1][0], 0);
                close(pfd[i-1][0]);

                execCommand(argv);

                // recovery redirect
                dup2(fd1, 1);
                dup2(fd0, 0);
            }

            i++;
        }
    }

    return 0;
}
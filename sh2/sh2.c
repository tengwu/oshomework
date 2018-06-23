#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SIZE 1024
char* buff;

void split(char** argv, char* argstr)
{
    if(argstr == NULL) {
        argv[0] = NULL;
        return ;
    }
    char *tmp;
    int len = strlen(argstr);
    int argi = 0;
    tmp = strtok(argstr, " ");
    while(tmp) {
        int len = strlen(tmp);
        argv[argi] = (char*)malloc(len+1);
        strcpy(argv[argi], tmp);
        argi++;
        tmp = strtok(NULL, " ");
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
    int file;
    char c;
    if(argv[1] == NULL) {
        printf("Error command.\n");
        return ;
    }
    char* filename = argv[1];
    // printf("fname = %s\n", filename);
    file = open(filename, O_RDONLY);
    if(file == -1) {
        printf("Error when open file.\n");
        return ;
    }
    int flag = 0;
    flag = read(file, &c, sizeof(char));
    while(flag != -1 && flag != 0) {
        printf("%c", c);
        flag = read(file, &c, sizeof(char));
    }
    if(flag == -1) {
        printf("Error when read file.\n");
        return ;
    }
    close(file);
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

void redirect(char** argv)
{
    int i = 0;
    while(argv[i]) {
        if(!strcmp(argv[i], ">")) {
            oRedirect(argv[i+1]);
            argv[i] = NULL;
        } else if(!strcmp(argv[i], "<")) {
            iRedirect(argv[i+1]);
            argv[i] = NULL;
        }
        i++;
    }
}

int main(int argc, char *argv[])
{
    buff = (char*)malloc(SIZE);
    while(1) {
        printf("$ ");
        fgets(buff, SIZE, stdin);
        buff[strlen(buff)-1] = '\0';

        char* argv[25];
        split(argv, buff);

        // I/O redirect
        redirect(argv);

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
            printf("Unknown neibu or waibu command.\n");
        }

        // recovery redirect
        noRedirect();
    }

    return 0;
}
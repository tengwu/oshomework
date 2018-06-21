#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define SIZE 1024
char* buff;

void cd(char* path){
    chdir(path);
}

void pwd(){
    char cwd[105];
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
}

void ls(char* cmd){
    pid_t pid;
    pid = fork();
    if(pid == 0){
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        _exit(127);
    }
    else{
        wait(NULL);
    }
}

void echo(char* cmd){
    pid_t pid;
    pid = fork();
    if(pid == 0){
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        _exit(127);
    }
    else{
        wait(NULL);
    }
}

int main(int argc, char *argv[])
{
    buff = (char*)malloc(SIZE);
    while(1){
        printf("$ ");
        fgets(buff, SIZE, stdin);
        buff[strlen(buff)-1] = '\0';
        if(!strcmp(buff, "exit")){
            exit(0);
        }
        if(buff[0] == 'c' && buff[1] == 'd'){
            cd(buff+3);
        }
        else if(!strcmp(buff, "pwd")){
            pwd();
        }
        else if(buff[0] == 'l' && buff[1] == 's'){
            ls(buff);
        }
        else if(buff[0] == 'e' && buff[1] == 'c'){
            echo(buff);
        }
        else{
            printf("Unknown neibu or waibu command.\n");
        }
    }

    return 0;
}

#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

int mysys(char* cmd){
    pid_t pid;
    int ret = 1;
    pid = fork();
    if(pid < 0){
        ret = -1;
    }
    else if(pid == 0){
        ret = execl("/bin/sh", "sh", "-c", cmd, NULL);
        _exit(127);
    }
    else{
        int error = wait(NULL);
        if(error < 0){
            printf("Some error ocured.\n");
        }
    }

    return ret;
}

int main(int argc, char* argv[])
{
    printf("--------------------------------------------------\n");
    mysys("echo HELLO WORLD");
    printf("--------------------------------------------------\n");
    mysys("ls /");
    printf("--------------------------------------------------\n");

    return 0;
}

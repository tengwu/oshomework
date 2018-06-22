#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
	// int fd = open("input.txt", O_RDONLY, 0664);
	// dup2(fd, 0);
	// pid_t pid = fork();
	// char* argv[25];
	// char s1[] = "cat";
	// argv[0] = s1; argv[1] = NULL;
	// if(pid == 0){
	// 	execvp("cat", argv);
	// }
	// else{
	// 	wait(NULL);
	// }

	int pfd[2];
	pipe(pfd);
	// pipe(pfd[1]);
	int i = 0;
int f_1 = dup(1);
		int f_0 = dup(0);
	for(i = 0 ; i < 2;i++){
		
		if(i == 0){
			// redirect write to

			dup2(pfd[1], 1);
			close(pfd[1]);
			// write to 1
			pid_t pid = fork();
			if(pid == 0){
				char* argv[25];
				argv[0] = "echo";
				argv[1] = "afdsafds";
				argv[2] = NULL;
				execvp("echo", argv	);
			}
			else{
				wait(NULL);
			}
			dup2(f_1, 1);
			close(f_1);
		}
		else{
			// redirect input
			dup2(pfd[0], 0);
			close(pfd[0]);
			pid_t pid = fork();
			if(pid == 0){
				char* argv[2];
				argv[0] = "cat";
				argv[1] = NULL;
				execvp("cat", argv);
			}
			else{
				wait(NULL);
			}
			dup2(f_0, 0);
			close(f_0);
		}
	}
	
	return 0;
}
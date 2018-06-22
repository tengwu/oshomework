#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

const int MAX_CMD = 1e3 + 5;
const int MAX_TOKEN = 1e2 + 5;
const int MAX_PROCESS = 15;

void do_execvp(char *token[], int tks){
	int pid = fork();
	if (pid < 0) {
		printf("fail to fork.\n");
		return;
	} else if (pid == 0) {
		
		if (strcmp(token[0],"echo") == 0) {
			if (token[tks-1][0] == '>') {
				int fd = open(token[tks-1]+1,O_WRONLY|O_TRUNC|O_CREAT,777);
				dup2(fd,1);
				close(fd);
				token[tks-1] = NULL;
				tks--;
			}
		}
		if (strcmp(token[0],"cat") == 0) {
			if (token[tks-1][0] == '>') {
				int fd = open(token[tks-1]+1,O_WRONLY|O_TRUNC|O_CREAT,777);
				dup2(fd,1);
				close(fd);
				token[tks-1] = NULL;
				tks--;
			}
		}

		int error;
		if ((error = execvp(token[0],token)) < 0) {
			printf("Errors.\n");
			exit(0);
		}
	} else {
		wait(NULL);
	}
}


int main() {
	char s[MAX_PROCESS * MAX_CMD], *cmd[MAX_CMD];
	int in, out;
	in = dup(0);
	out = dup(1);
	while (1) {
		
		printf("$ ");
		fgets(s, MAX_PROCESS * MAX_CMD - 1, stdin);
		if (s[strlen(s) - 1] == '\n') {
			s[strlen(s) - 1] = '\0';
		}
		if (s == NULL || !strcmp(s, " ") || !strcmp(s, "\n"))	continue;
		
		int cmds = 1;
		char *p;
		cmd[0] = strtok(s, "|");
		while (p = strtok(NULL, "|")) {
			cmd[cmds++] = p;
		}
		if (cmds == 1) {
			
		} else {
			int fd[MAX_PROCESS][2],tks;
			char *token[MAX_TOKEN];
			int i=0;
			for(i=0;i<cmds;i++){
				tks = 1;
				token[0] = strtok(cmd[i], " ");
				while (p = strtok(NULL, " ")) {
					token[tks++] = p;
				}
				token[tks] = NULL;
				if(i==0) {
					pipe(fd[i]);
					dup2(fd[i][1], 1);
					close(fd[i][1]);
					int pid = fork();
					if (pid == 0) {
						// puts(token[0]);
						do_execvp(token,tks);
						// execvp(token[0],token);
						exit(0);
					}
					wait(NULL);
				}else if(i==cmds-1){
					dup2(out, 1);
					dup2(fd[i-1][0],0);
					close(fd[i-1][0]);
					int pid = fork();
					if (pid == 0) {
						// puts(token[0]);
						// pipe(fd[i]);
						
						// close(fd[i][0]);
						// close(fd[i][1]);
						do_execvp(token,tks);
						exit(0);
					}
					wait(NULL);
				}else{
					pipe(fd[i]);
					dup2(fd[i-1][0],0);
					dup2(fd[i][1],1);
					close(fd[i-1][0]);
					close(fd[i][1]);
					int pid = fork();
					if (pid == 0) {
						do_execvp(token,tks);
						exit(0);
					}
					wait(NULL);
				}
			}
		}
		dup2(in, 0);
		dup2(out, 1);
	}

	return 0;
}
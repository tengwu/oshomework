#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

const int CMD_MAX = 1024;
const int PATH_MAX = 200;

int spilt(char* buf,char *argv[])
{
	int argc = 0;
	argv[argc++] = strtok(buf," ");
	while(argv[argc-1] != NULL)
	{
		argv[argc++] = strtok(NULL," ");
	}
	return argc;
}
void file_redirection_write(char *path,char *buf)
{
	if(buf[0]=='\0') return;
	int pid = fork();
	if(pid == 0) 
	{
		if(path == NULL) puts(buf);
		else {
			int fd = open(path,O_WRONLY | O_CREAT | O_TRUNC,0777);
			dup2(fd,1);
			close(fd);
			write(1,buf,strlen(buf));
			write(1,"\n",1);
			close(1);
		}
		exit(0);
	}
	wait(NULL);
}
void file_redirection_read(char *path,char *buf)
{
	int fd = open(path,O_RDONLY | O_CREAT,0777);
	int org = dup(0);	
	dup2(fd,0);
	close(fd);
	read(0,buf,CMD_MAX);
	dup2(org,0);
	close(org);
}
void pipe_write_by_path(char *path,int fd[])
{
	int pid = fork();
	if(pid == 0)
	{				
		dup2(fd[1],1);	
		close(fd[1]);
		execlp("cat","cat",path,NULL);
		exit(0);
	}
	wait(NULL);
}
void pipe_write_by_buf(char *buf,int fd[])
{
	int pid = fork();
	if(pid == 0)
	{
		dup2(fd[1],1);
		close(fd[1]);
		write(1,buf,strlen(buf));
		exit(0);
	}
	wait(NULL);
}
void pipe_read_to_buf(char *buf,int fd[])
{
	int org = dup(0);	
	dup2(fd[0],0);
	close(fd[0]);
	read(0,buf,CMD_MAX);
	int len = strlen(buf);
	if(buf[len-1] == '\n') buf[len-1]='\0';
	dup2(org,0);
	close(org);
}
 
void pipe_sort(int fd[])
{
	int pid = fork();
	if(pid == 0)
	{
		
		char buf[CMD_MAX];
		memset(buf,'\0',sizeof(buf));
		pipe_read_to_buf(buf,fd);
		char *str[CMD_MAX];
		int num = 0;
		char *tmp = strtok(buf,"\n");
		while(tmp)
		{
			str[num++] = tmp;
			tmp = strtok(NULL,"\n");
		}
		for(int i=0;i<num;i++)
			for(int j=1;j<num;j++)
				if(strcmp(str[j-1],str[j]) > 0)
				{
					stpcpy(tmp,str[j]);
					stpcpy(str[j],str[j-1]);
					stpcpy(str[j-1],tmp);
				}	

		for(int i=0;i<num;i++)
			pipe_write_by_buf(str[i],fd);
		exit(0);
	}
	wait(NULL);
}
void pipe_unique(int fd[])
{
	int pid = fork();
	if(pid == 0)
	{
		char buf[CMD_MAX];
		memset(buf,'\0',sizeof(buf));
		pipe_read_to_buf(buf,fd);
		char *str[CMD_MAX];
		int num = 0;
		char *tmp = strtok(buf,"\n");
		while(tmp)
		{
			str[num++] = tmp;
			tmp = strtok(NULL,"\n");
		}
		pipe_write_by_buf(str[0],fd);
		for(int i=1;i<num;i++)
			if(strcmp(str[i],str[i-1]) != 0)  pipe_write_by_buf(str[i],fd);
		exit(0);
	}
	wait(NULL);
}
void pipe_wc(char *argv[],int fd[])
{
	int pid = fork();
	if(pid == 0)
	{		
		dup2(fd[0],0);
		close(fd[0]);

		char* argv[] = {"wc", "-l", NULL};
		execvp("wc", argv);	
		exit(0); 
	}
	wait(NULL);
}
void pipe_tech(int argc,char *argv[],int fd[])
{
	if(argc==0 || argv[0] == NULL) return;
	if(strcmp(argv[0],"cat") == 0)
	{
		char *path =  (char*)malloc((CMD_MAX)*sizeof(char));
		if(argv[1][0] == '<')
		{
			path = strtok(argv[1],"<");
			char buf[CMD_MAX];
			memset(buf,'\0',sizeof(buf));			
			file_redirection_read(path,buf);			
			pipe_write_by_buf(buf,fd);
			//pipe_write_by_path(path,fd);
		}
		else if(argv[1][0] == '>')
		{
			path = strtok(argv[1],">");
			char buf[CMD_MAX];
			memset(buf,'\0',sizeof(buf));
			pipe_read_to_buf(buf,fd);
			file_redirection_write(path,buf);
		}
		else {
			strcpy(path,argv[1]);
			char buf[CMD_MAX];
			memset(buf,'\0',sizeof(buf));			
			file_redirection_read(path,buf);			
			pipe_write_by_buf(buf,fd);
			//pipe_write_by_path(path,fd);
		}
		if(argv[2] != NULL && strcmp(argv[2],"|") == 0) pipe_tech(argc-3,argv+3,fd); 
	}
	else if(strcmp(argv[0],"sort") == 0) 
	{
		pipe_sort(fd);
		if(argv[1] != NULL && strcmp(argv[1],"|") == 0) pipe_tech(argc-2,argv+2,fd); 
	}
	else if(strcmp(argv[0],"unique") == 0)
	{
		pipe_unique(fd);
		if(argv[1] != NULL && strcmp(argv[1],"|") == 0) pipe_tech(argc-2,argv+2,fd); 
	}
	else if(strcmp(argv[0],"wc") == 0)
	{
		pipe_wc(argv,fd);	
	}
}
int outer_command(int argc,char *argv[])
{
	
	if(strcmp(argv[0],"echo") == 0)
	{
		char *path,buf[CMD_MAX];
		buf[0]='\0';
		path=NULL;
		for(int i=1;i<argc-1;i++)
		{
			if(strcmp(argv[i],">") == 0 && i+1<argc-1)
			{
				path = (char*)malloc((CMD_MAX)*sizeof(char));			
				strcpy(path,argv[i+1]);
				break;
			}
			else if(argv[i][0] == '>')
			{
				path = (char*)malloc((CMD_MAX)*sizeof(char));				
				path = strtok(argv[i],">");
				break;
			}
			else 
			{			
				if(i!=1) strcat(buf," ");
				strcat(buf,argv[i]);
			}
		}
		file_redirection_write(path,buf);
	}
	else if(strcmp(argv[0],"cat") == 0 && argc > 3)
	{
		int pid = fork();
		if(pid == 0)
		{
			int fd[2];
			pipe(fd);
			pipe_tech(argc,argv,fd);
			exit(0);
		}		
		wait(NULL);
	}
	else {
		int pid = fork();
		if(pid == 0)
		{
			execvp(argv[0],argv);
			exit(127); 
		}
		wait(NULL);
	}
	return 0;
}

int inner_command(char* argv[])
{
	if(strcmp(argv[0],"cd") == 0)
	{
		chdir(argv[1]);
		return 1;
	}
	else if(strcmp(argv[0],"pwd") == 0)
	{
		char buf[PATH_MAX+1];
		printf("%s\n",getcwd(buf,PATH_MAX));
		return 1;
	}
	else if(strcmp(argv[0],"exit") == 0)
	{
		exit(0);
	}
	return 0;
}

void eval(const char *command)
{
	int argc;
	char *argv[CMD_MAX];
	char buf[CMD_MAX];
	strcpy(buf,command);
	argc = spilt(buf,argv);
	if(!argv[0]) return;
	if(inner_command(argv)) return;
	//if(file_redirection(argc,argv)) return;
	
	outer_command(argc,argv);
}

int main()
{
	char command[CMD_MAX+1];
	while(1)
	{
		printf("$ ");
		fflush(stdout);
		memset(command,'\0',sizeof(command));
		if(read(0,command,CMD_MAX) == -1)
		{
			perror("read fail");
		}
		int len = strlen(command);
		command[len-1] = '\0';
		eval(command);
	}
	return 0;
}
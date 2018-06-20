#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    if(argc == 1)
        return 0;
    char* filename = argv[1];
    int file;
    char c;
    file = open(filename, O_RDONLY);
    if(file == -1){
        printf("Error when open file.\n");
        return 0;
    }
    int flag = 0;
    flag = read(file, &c, sizeof(char));
    while(flag != -1 && flag != 0){
        printf("%c", c);
        flag = read(file, &c, sizeof(char));
    }
    if(flag == -1){
        printf("Error when read file.\n");
        return 0;
    }
    close(file);

    return 0;
}

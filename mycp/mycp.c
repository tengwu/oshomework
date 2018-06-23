#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    if(argc != 3) {
        printf("can shu bu dui a.\n");
        return 0;
    }
    char* ori_filename = argv[1];
    char* dest_filename = argv[2];
    int ori_file, dest_file;
    ori_file = open(ori_filename, O_RDONLY);
    if(ori_file == -1) {
        printf("Error when open file.\n");
        return 0;
    }
    dest_file = creat(dest_filename, 0664);
    if(dest_file == -1) {
        printf("Error when create file.\n");
        return 0;
    }
    char buf;
    int flag = 0;
    while((flag = read(ori_file, &buf, sizeof(char))) && flag != 0 && flag != -1) {
        int flagw = write(dest_file, &buf, sizeof(char));
        if(flagw == -1) {
            printf("Error when write content to %s.\n", dest_filename);
            return 0;
        }
    }
    if(flag == -1) {
        printf("Error when cp file.\n");
        return 0;
    }
    printf("Successfully cp file from %s to %s.\n", ori_filename, dest_filename);
    close(ori_file);
    close(dest_file);

    return 0;
}

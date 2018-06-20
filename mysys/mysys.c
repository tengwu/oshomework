#include <stdio.h>
#include <unistd.h>

int main()
{
    puts("before exec");
    int error = execl("/bin/echo", "echo","a", "b", "c", NULL);
    if(error < 0){
        perror("execl");
    }

    return 0;
}

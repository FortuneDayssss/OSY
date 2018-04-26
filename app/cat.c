#include "stdio.h"
#include "fs.h"
#include "string.h"
#include "global.h"

#define BUFFER_SIZE 1024

int main(int argc, char** argv){
    if(argc != 2){
        printf("use cat [path]\n");
        return 0;
    }
    char* path = argv[1];
    int fd = open(path, O_RDWR);
    if(fd < 0){
        printf("cat error: file not exist\n");
        return 0;
    }
    File_Stat file_stat;
    stat(path, &file_stat);
    int remain_size = file_stat.size;
    char buffer[BUFFER_SIZE];
    

    do{
        memset(buffer, 0, sizeof(char) * BUFFER_SIZE - 1);
        remain_size -= read(fd, buffer, min(remain_size, BUFFER_SIZE - 1));
        buffer[BUFFER_SIZE - 1] = '\0';
        printf("%s", buffer);
    }while(remain_size > 0);
    printf("\n");

    close(fd);
    return 0;
}
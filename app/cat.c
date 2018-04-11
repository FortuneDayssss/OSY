#include "stdio.h"
#include "fs.h"
#include "string.h"
#include "global.h"

int main(int argc, char** argv){
    if(argc != 2){
        printf("use cat [path]\n");
        return 0;
    }
    char* path = argv[1];
    File_Stat file_stat;
    stat(path, &file_stat);
    int file_size = file_stat.size;
    char buffer[1024];
    memset(buffer, 0, sizeof(char) * 1024);
    int fd = open(path, O_RDWR);
    read(fd, buffer, file_size);
    close(fd);
    printf("%s\n", buffer);
    return 0;
}
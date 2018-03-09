#include "stdio.h"
#include "fs.h"
#include "string.h"
#include "global.h"

int main(int argc, char** argv){
    if(argc != 2){
        printf("use ls [path]\n");
        return 0;
    }
    char* path = argv[1];
    File_Stat file_stat;
    stat(path, &file_stat);
    int file_size = file_stat.size;
    Dir_Entry dir_entry_table[20];
    memset(dir_entry_table, 0, sizeof(Dir_Entry) * 20);
    int fd = open(path, O_RDWR);
    read(fd, dir_entry_table, min(file_size, 20 * sizeof(Dir_Entry)));
    close(fd);
    for(int i = 0; i < min(file_size/sizeof(Dir_Entry), 20); i++){
        printf("%s ", dir_entry_table[i].file_name);
    }
    printf("\n");
    return 0;
}
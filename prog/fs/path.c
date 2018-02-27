#include "type.h"
#include "string.h"
#include "fs.h"
#include "global.h"
#include "hd.h"
#include "print.h"

int strcmp_fix(char* s1, char* s2, int len){
    // printString("str len: ", -1);printInt32(len);
    int i;
    int equal_flag = 1;
    for(i = 0; i < len; i++){
        if(*(s1 + i) == '\0' || *(s2 + i) == '\0'){
            if(*(s1 + i) == *(s2 + i))
                return 0;
            else{
                // printString("AAA ", -1);printString(s1 + i, 1);printString(" != ", -1);printString(s2 + i, 1);printString("\n", 1);
                return -1;
            }
        }

        if(*(s1 + i) != *(s2 + i)){
            // printString(s1 + i, 1);printString(" != ", -1);printString(s2 + i, 1);printString("\n", 1);
            return -1;
        }
    }
    return 0;
}

int get_filename_len_in_path(char* p, int max_len){
    for(int i = 0; i < max_len; i++)
        if(*(p + i) == '/' || *(p + i) == '\0')
            return i;
    return max_len;
}

int search_file(char* path){
    int path_len = strlen(path);
    char* cp = path;
    int file_name_len;
    Dir_Entry* files_in_dir;
    Dir_Entry* founded_file;
    uint8_t* buf[SECTOR_SIZE];
    char temp_file_name[MAX_FILENAME_LEN];
    INode* inode_ptr;
    int nr_file_inode;
    if(*cp == '/'){ // absolute address
        cp++;
        nr_file_inode = INODE_ROOT;
        if(*cp == '\0'){
            return nr_file_inode;
        }

        while(1){
            inode_ptr = get_inode(MAKE_DEV(DEV_HD, 1), nr_file_inode);
            int sectors = inode_ptr->file_size / SECTOR_SIZE + 1;
            founded_file = 0;
            printString("SECTORSSSS: ", -1);printInt32(sectors);printString("\n", -1);

            for(int i = 0, founded; i < sectors; i++){
                founded = 0;
                read_sector(inode_ptr->nr_start_sector + i, buf, SECTOR_SIZE);
                file_name_len = get_filename_len_in_path(cp, path_len - (cp - path));

                files_in_dir = (Dir_Entry*)buf;
                for(int de_index_in_sec = 0; de_index_in_sec < SECTOR_SIZE / DIR_ENTRY_SIZE; de_index_in_sec++){
                    // printString("find filename: ", -1);printString(cp, -1);printString("\n", -1);
                    printString("iterate filename: ", -1);printString(files_in_dir[de_index_in_sec].file_name, -1);printString("\n", -1);
                    if(files_in_dir[i].nr_inode != INODE_INVALID && (strcmp_fix(cp, files_in_dir[de_index_in_sec].file_name, file_name_len) == 0)){
                        printString("MATCH!!!\n", -1);
                        printString(cp, file_name_len);printString(" = ", -1);printString(files_in_dir[de_index_in_sec].file_name, -1);printString("\n", -1);
                        founded = 1;
                        founded_file = &(files_in_dir[de_index_in_sec]);
                        break;
                    }
                }

                if(founded){
                    break;
                }
            }

            if(!founded_file){
                error_log("cannot find file");
                return 0;
            }
            else{
                nr_file_inode = founded_file->nr_inode;
                printString("founded file inode nr: ", -1);printInt32(nr_file_inode);printString("\n", -1);
                cp += file_name_len;
                if(*cp == '/' && *(cp + 1) != '\0'){ // next step
                    cp++;
                    continue;
                }
                
                if(*cp == '\0' || (*cp == '/' && *(cp + 1) == '\0')){ // the file was founded
                    debug_log("DDDDDD");
                    return nr_file_inode;
                }
                else{
                    error_log("invalid path");
                    return 0;
                }
            }

            // read_sector(inode_ptr->nr_start_sector, buf, SECTOR_SIZE);
            // files_in_dir = (Dir_Entry*)buf;
            // file_name_len = get_filename_len_in_path(cp, path_len - (cp - path));
            // printString("file name len to compare: ", -1);printInt32(file_name_len);printString("\n", -1);
            // printString("first dir entry file name: ", -1);printString(files_in_dir->file_name, -1);printString("\n", -1);
            // printInt32(*((uint32_t*)buf));
            // // printInt32(inode_ptr->nr_start_sector);
            // while(files_in_dir->nr_inode != INODE_INVALID){
            //     // printString("find filename: ", -1);printString(cp, -1);printString("\n", -1);
            //     // printString("iterate filename: ", -1);printString(files_in_dir->file_name, -1);printString("\n", -1);
                
            //     if(strcmp_fix(cp, files_in_dir->file_name, file_name_len) == 0){
            //         printString(cp, 7);printString(" = ", -1);printString(files_in_dir->file_name, -1);printString("\n", -1);
            //         break;
            //     }
            //     files_in_dir++;
            // }
            // if(files_in_dir->nr_inode != INODE_INVALID){
            //     nr_file_inode = files_in_dir->nr_inode;
            //     cp += file_name_len;
            //     if(*cp == '/'){ // next step
            //         cp++;
            //         continue;
            //     }
                
            //     if(*cp == '\0' || (*cp == '/' && *(cp + 1) == '\0')){ // the file was founded
            //         return nr_file_inode;
            //     }
            //     else{
            //         return 0;
            //     }
            // }
            // else{ // cannot find file
            //     return 0;
            // }
        }
    }
    else{ // relative address (todo!!!
        return 0;
    }

    return 0;
}

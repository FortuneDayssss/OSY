#include "ipc.h"
#include "message.h"
#include "string.h"
#include "process.h"
#include "global.h"
#include "stdio.h"
#include "print.h"
#include "hd.h"

INode* create_file(char* path, int flags);

int do_open(Message* msg){
    int fd = -1;
    
    // get data from message
    uint32_t path_name_len = msg->mdata_fs_open.path_name_len;
    if(path_name_len > MAX_FILENAME_LEN){
        debug_log("path length is too long");
        return fd;
    }
    char path_name[MAX_FILENAME_LEN];
    memcpy(path_name, (void*)msg->mdata_fs_open.path_name, path_name_len);
    path_name[path_name_len] = 0;
    int flags = msg->mdata_fs_open.flags;

    // find an available file descriptor ptr in src process's filp table
    PCB* src_proc = &pcb_table[msg->src_pid];
    for(int i = 0; i < FILP_TABLE_SIZE; i++)
        if(src_proc->filp_table[i] == 0){
            fd = i;
            break;
        }
    if(fd < 0){ // filp table is full
        error_log("filp table is full");        
        return -1;
    }

    // find an available file descriptor in global fd table
    File_Descriptor* fd_ptr = 0;
    for(int i = 0; i < NR_FILE_DESCRIPTOR; i++){
        if(fd_table[i].fd_inode == 0){
            fd_ptr = &fd_table[i];
            break;
        }
    }
    if(!fd_ptr){
        error_log("fd table is full");        
        return -1;
    }
    
    // find inode by file path
    uint32_t inode_nr = search_file(path_name);
    printString("Search File OK, INode nr = ", -1);printInt32(inode_nr);printString("\n", -1);
    INode* inode_ptr = 0;
    if(flags & O_CREATE){
        if(inode_nr != INODE_INVALID){
            // file exists
            error_log("the file was exists when creating");        
            return -1;
        }
        else{
            inode_ptr = create_file(path_name, flags);
        }
    }
    else {
        if(inode_nr == INODE_INVALID){
            error_log("file not exist");
            return -1;
        }
        if(flags & O_RDWR){
            inode_ptr = get_inode(MAKE_DEV(DEV_HD, 1), inode_nr);
        }
        else{ // cannot get file open mode
            error_log("error open flag");        
            return -1;
        }
    }

    // link [filp => fd => inode] and process access mode
    if(inode_ptr){ // get inode success
        src_proc->filp_table[fd] = fd_ptr;
        fd_ptr->fd_inode = inode_ptr;
        fd_ptr->fd_mode = flags;
        fd_ptr->fd_pos = 0;

        if((fd_ptr->fd_mode & ACCESS_MODE_TYPE_MASK) == ACCESS_MODE_CHAR_SPECIAL){// tty file
            // todo: open tty device
        }
        else if((fd_ptr->fd_mode & ACCESS_MODE_TYPE_MASK) == ACCESS_MODE_DIRECTORY){// dir file
            //...
        }
        else if((fd_ptr->fd_mode & ACCESS_MODE_TYPE_MASK) == ACCESS_MODE_REGULAR){// regular file
            //...
        }
    }
    else{ // get inode fail
        error_log("do_open get inode fail");
        return -1;
    }
    return fd;
}


INode* create_file(char* path, int flags){
    char dir_name[MAX_FILENAME_LEN];
    char file_name[MAX_FILENAME_LEN];
    char* dp = dir_name;
    char* fp = file_name;
    INode* dir_inode_ptr;
    int path_len = strlen(path);

    // copy dir name and file name from path
    int last_sep_index = -1;
    for(int i = 0; i < path_len; i++){
        if(path[i] == '/' && path[i + 1] != '\0')
            last_sep_index = i;
    }
    if(last_sep_index == -1) // invalid path
        return 0;
    
    for(int i = 0; i < path_len; i++){
        if(i <= last_sep_index)
            *dp++ = path[i];
        else if(i > last_sep_index)
            *fp++ = path[i];
    }
    *dp = '\0';
    *fp = '\0';

    // 1. alloc imap-bitmap, smap-bitmap, inode 
    // 2. put new_inode_nr into dir's dir_entry_table
    // 3. return new inode pointer
    int dir_inode_nr = search_file(dir_name);
    dir_inode_ptr = get_inode(MAKE_DEV(DEV_HD, 1), dir_inode_nr);
    printString("DDDIR_INODE_NR: ", -1);printInt32(dir_inode_ptr->nr_inode);printString("\n", -1);
    int new_inode_nr = alloc_imap(MAKE_DEV(DEV_HD, 1));
    printString("new inode number: ", -1);printInt32(new_inode_nr);printString("\n", -1);
    int new_sectors_start_nr = alloc_smap(MAKE_DEV(DEV_HD, 1), NR_DEFAULT_FILE_SECTORS);
    printString("new sectors start number: ", -1);printInt32(new_sectors_start_nr);printString("\n", -1);
    INode* new_inode = init_inode(MAKE_DEV(DEV_HD, 1), new_inode_nr, new_sectors_start_nr);
    printString("init inode ok!\n", -1);
    new_dir_entry(dir_inode_ptr, new_inode_nr, file_name);

    return new_inode;
}

int alloc_imap(uint32_t dev){
    Super_Block* sb = get_super_block(dev);
    int nr_imap_start_sector = 1 + 1;
    uint8_t buf[SECTOR_SIZE];
    
    for(int i = 0; i < sb->nr_imap_sectors; i++){
        read_sector(nr_imap_start_sector + i, buf, SECTOR_SIZE);
        for(int j = 0; j < SECTOR_SIZE; j++){
            // this byte is full
            if(buf[j] == 0xFF)
                continue;
            
            // have available bit
            int bit_index = 0;
            while(((buf[j] >> bit_index) & 1) != 0)
                bit_index++;
            buf[j] |= (1 << bit_index);
            write_sector(nr_imap_start_sector + i, buf, SECTOR_SIZE);
            return (SECTOR_SIZE * i + j) * 8 + bit_index;
        }
    }

    // inode bitmap is full
    error_log("inode bitmap is full");
    return INODE_INVALID;
}

int alloc_smap(uint32_t dev, int nr_sectors){
    Super_Block* sb = get_super_block(dev);
    int nr_smap_start_sector = 1 + 1 + sb->nr_imap_sectors;
    uint8_t buf[SECTOR_SIZE];
    int free_sect_nr = 0;
    int bit_index;
    int nr_remain = nr_sectors;

    for(int i = 0; i < sb->nr_smap_sectors; i++){
        read_sector(nr_smap_start_sector + i, buf, SECTOR_SIZE);
        for(int j = 0; j < SECTOR_SIZE && nr_remain > 0; j++){
            bit_index = 0;
            if(!free_sect_nr){
                if(buf[j] == 0xFF)
                    continue;
                while(((buf[j] >> bit_index) & 1) != 0)
                    bit_index++;
                free_sect_nr = sb->nr_first_data_sector + 
                    (SECTOR_SIZE * i + j) * 8 + bit_index - 1;
            }

            for(; bit_index < 8; bit_index++){
                buf[j] |= (1 << bit_index);
                if(--nr_remain <= 0)
                    break;
            }
        }
        if(free_sect_nr)
            write_sector(nr_smap_start_sector + i, buf, SECTOR_SIZE);
        if(nr_remain <= 0)
            break;
    }
    return free_sect_nr;
}


INode* init_inode(uint32_t dev, uint32_t inode_nr, uint32_t start_sector_nr){
    INode* new_inode = get_inode(dev, inode_nr);

    new_inode->access_mode = ACCESS_MODE_REGULAR;
    new_inode->file_size = 0;
    new_inode->nr_start_sector = start_sector_nr;
    new_inode->nr_sectors = NR_DEFAULT_FILE_SECTORS;

    new_inode->dev_no = dev;
    new_inode->process_counter = 1;
    new_inode->nr_inode = inode_nr;
    sync_inode(new_inode);
    
    return new_inode;
}

void* sync_inode(INode* inode_ptr){
    // calculate the inode's sector in disk
    Super_Block* sb = get_super_block(inode_ptr->dev_no);
    int block_nr = 1 + 1 + sb->nr_imap_sectors + sb->nr_smap_sectors + 
        ((inode_ptr->nr_inode - 1) / (SECTOR_SIZE / INODE_SIZE));

    // load sector, sync data, write sector
    uint8_t buf[SECTOR_SIZE];
    read_sector(block_nr, buf, SECTOR_SIZE);
    INode* write_inode_ptr = 
        (INode*)(buf + (((inode_ptr->nr_inode - 1) % (SECTOR_SIZE / INODE_SIZE)) * INODE_SIZE));
    write_inode_ptr->access_mode = inode_ptr->access_mode;
    write_inode_ptr->file_size = inode_ptr->file_size;
    write_inode_ptr->nr_start_sector = inode_ptr->nr_start_sector;
    write_inode_ptr->nr_sectors = inode_ptr->nr_sectors;
    write_sector(block_nr, buf, SECTOR_SIZE);
}

void new_dir_entry(INode* dir_inode, int file_inode_nr, char* file_name){
    printString("DIR_INODE_NUMBER: ", -1);printInt32(dir_inode->nr_inode);printString("\n", -1);
    int dir_sector_start_nr = dir_inode->nr_start_sector;
    int dir_sector_nr = dir_inode->file_size / SECTOR_SIZE + 1;
    int dir_entry_table_size = dir_inode->file_size / DIR_ENTRY_SIZE;
    printString("DEBUG: DIR_FILE_SIZE: ", -1);printInt32(dir_entry_table_size);printString("\n", -1);

    Dir_Entry* dir_entry_table_ptr;
    Dir_Entry* new_de_ptr = 0;
    uint8_t buf[SECTOR_SIZE];
    int counter = 0;
    int i;
    for(i = 0; i < dir_sector_nr; i++){
        printString("STARTTT_SECTORRR: ", -1);printInt32(dir_inode->nr_start_sector + i);printString("\n", -1);
        read_sector(dir_inode->nr_start_sector + i, buf, SECTOR_SIZE);
        for(int j = 0; j < SECTOR_SIZE / DIR_ENTRY_SIZE; j++){
            dir_entry_table_ptr = (Dir_Entry*)buf + j;
            if(++counter > dir_entry_table_size){
                debug_log("EEEEEE1");
                break;
            }
            if(dir_entry_table_ptr->nr_inode == INODE_INVALID){
                debug_log("EEEEEE2");
                new_de_ptr = dir_entry_table_ptr;
                break;
            }
        }
        if(counter > dir_entry_table_size || new_de_ptr){
            debug_log("EEEEEE3");
            break;
        }
    }
    
    if(!new_de_ptr){// dir entry table is full, extend file size and save new dir entry
        debug_log("dir_entry_table is full, extend!~");
        new_de_ptr = dir_entry_table_ptr;
        dir_inode->file_size += DIR_ENTRY_SIZE;
    }
    printString("index: ", -1);printInt32(new_de_ptr - (Dir_Entry*)buf);printString("\n", -1);

    new_de_ptr->nr_inode = file_inode_nr;
    printString("file_inode_nr: ", -1);printInt32(file_inode_nr);printString("\n", -1);    
    
    strcpy(new_de_ptr->file_name, file_name);

    write_sector(dir_inode->nr_start_sector + i, buf, SECTOR_SIZE);

    sync_inode(dir_inode);
}
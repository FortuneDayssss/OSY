#ifndef _FS_H__
#define _FS_H__

#include "type.h"
#include "ipc.h"

#define NR_DEFAULT_FILE_SECTORS 2048

#define FS_OSY_MAGIC_V1         0xBBA

#define ACCESS_MODE_TYPE_MASK       0170000
#define ACCESS_MODE_REGULAR         0100000
#define ACCESS_MODE_BLOCK_SPECIAL   0060000
#define ACCESS_MODE_DIRECTORY       0040000
#define ACCESS_MODE_CHAR_SPECIAL    0020000
#define ACCESS_MODE_NAMED_PIPE      0010000

typedef struct{
    // load from hard disk (56 Byte)
    uint32_t    magic;
    uint32_t    nr_inodes;
    uint32_t    nr_sectors;
    uint32_t    nr_imap_sectors;
    uint32_t    nr_smap_sectors;
    uint32_t    nr_first_data_sector;
    uint32_t    nr_inode_sectors;
    uint32_t    root_inode;
    uint32_t    inode_size;
    uint32_t    inode_file_size_offset;
    uint32_t    inode_start_sector_offset;
    uint32_t    dir_entry_size;
    uint32_t    dir_entry_inode_offset;
    uint32_t    dir_entry_fname_offset;

    // save in memory
    uint32_t    dev_no;
}Super_Block;
#define NR_SUPER_BLOCK      8
#define SUPER_BLOCK_SIZE    56      // size of super block below

typedef struct{
    // load from hard disk (32 Byte)
    uint32_t    access_mode;
    uint32_t    file_size;
    uint32_t    nr_start_sector;
    uint32_t    nr_sectors;
    uint8_t     _unsesd_for_align[16];

    // save in memory
    uint32_t    dev_no;
    uint32_t    process_counter;
    uint32_t    nr_inode;
}INode;
#define NR_INODE            64
#define INODE_SIZE          32
#define ROOT_INODE          1

#define MAX_FILENAME_LEN    60
#define MAX_FILEPATH_LEN    128
typedef struct{
    uint32_t    nr_inode;
    char        file_name[60];
}Dir_Entry;
#define DIR_ENTRY_SIZE      sizeof(Dir_Entry)


typedef struct {
    uint32_t    fd_mode;
    uint32_t    fd_pos;
    INode*      fd_inode;
}File_Descriptor;
#define NR_FILE_DESCRIPTOR  64


// fs/fs_main.c
// extern uint8_t* fs_buf;
void fs_main();
void read_sector(uint32_t sector, void* buf, uint32_t len);
void write_sector(uint32_t sector, void* buf, uint32_t len);
INode* get_inode(uint32_t dev, int nr_inode);
INode* put_inode(INode* inode_ptr);
Super_Block* get_super_block(uint32_t dev);

// fs/open.c
void* sync_inode(INode* inode_ptr);
int alloc_imap(uint32_t dev);
int alloc_smap(uint32_t dev, int nr_secotrs);
INode* init_inode(uint32_t dev, uint32_t inode_nr, uint32_t start_sector_nr);
void new_dir_entry(INode* dir_inode, int file_inode_nr, char* file_name);
int do_open(Message* msg);
int do_close(Message* msg);

// fs/read_write.c
int do_read(Message* msg);
int do_write(Message* msg);

// fs/path.c
int search_file(char* path);

#endif
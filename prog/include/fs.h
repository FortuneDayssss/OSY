#ifndef _FS_H__
#define _FS_H__

#include "type.h"

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

#define MAX_FILENAME_LEN    64
typedef struct{
    uint32_t    nr_inode;
    char        file_name[64];
}Dir_Entry;
#define DIR_ENTRY_SIZE      sizeof(Dir_Entry)


typedef struct {
    uint32_t    fd_mode;
    uint32_t    fd_pos;
    INode*      fd_inode;
}File_Descriptor;
#define NR_FILE_DESCRIPTOR  64

void fs_main();

#endif
#ifndef _FS_H__
#define _FS_H__

#include "type.h"

#define fs_osy_v1   0xBBA

typedef struct{
    // load from hard disk (56 Byte)
    uint32_t    magic;
    uint32_t    nr_inodes;
    uint32_t    nr_sectors;
    uint32_t    nr_imap_sectors;
    uint32_t    nr_smap_sectors;
    uint32_t    nr_first_data_sector;
    uint32_t    root_inode;
    uint32_t    inode_size;
    uint32_t    inode_isize_offset;
    uint32_t    inode_start_offset;
    uint32_t    dir_entry_size;
    uint32_t    dir_entry_inode_offset;
    uint32_t    dir_entry_fname_offset;

    // save in memory
    uint32_t    dev_no;
}Super_Block;
#define SUPER_BLOCK_SIZE    56      // size of super block below

typedef struct{
    // load from hard disk (32 Byte)
    uint32_t    access_mode;
    uint32_t    file_size;
    uint32_t    nr_start_sector;
    uint32_t    nr_sector;
    uint8_t     _unsesd_for_align[16];

    // save in memory
    uint32_t    dev_no;
    uint32_t    process_counter;
    uint32_t    nr_inode;
}INode;

#endif
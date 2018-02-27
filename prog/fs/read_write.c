#include "ipc.h"
#include "message.h"
#include "fs.h"
#include "hd.h"
#include "process.h"
#include "global.h"
#include "stdio.h"
#include "process.h"
#include "string.h"
#include "print.h"

int do_read(Message* msg){
    // get data from message
    uint32_t fd = msg->mdata_fs_read.fd;
    void* buf = (void*)(msg->mdata_fs_read.buf);
    uint32_t len = msg->mdata_fs_read.len;
    PCB* src_proc = &pcb_table[msg->src_pid];
    uint8_t sec_buf[SECTOR_SIZE];

    // check access mode
    if(!(src_proc->filp_table[fd]->fd_mode & O_RDWR)){
        return -1;
    }

    // prepare for copy
    INode* inode_ptr = src_proc->filp_table[fd]->fd_inode;
    int pos = src_proc->filp_table[fd]->fd_pos;
    int pos_end = (pos + len) < (inode_ptr->file_size) ? (pos + len) : (inode_ptr->file_size);


    if((inode_ptr->access_mode & ACCESS_MODE_TYPE_MASK) == ACCESS_MODE_CHAR_SPECIAL){
        //todo tty
        return 0;
    }
    else{ // copy from hd sec
        int sec_min = inode_ptr->nr_start_sector + (pos / SECTOR_SIZE);
        int sec_max = inode_ptr->nr_start_sector + (pos_end / SECTOR_SIZE);
        uint8_t* sec_buf_p = sec_buf + (pos % SECTOR_SIZE);
        uint8_t* user_buf_p = (uint8_t*)buf;
        int read_len_counter = 0;
        for(int sec = sec_min; sec <= sec_max; sec++){
            read_sector(sec, sec_buf, SECTOR_SIZE);
            int len_in_loop = min(SECTOR_SIZE - (pos % SECTOR_SIZE), inode_ptr->file_size - pos);
            printString("FILE SIZE: ", -1);printInt32(inode_ptr->file_size);printString("\n", -1);
            printString("LEN IN LOOP: ", -1);printInt32(len_in_loop);printString("\n", -1);
            printString("READ DATA: ", -1);printString((char*)sec_buf_p, len_in_loop);printString("\n", -1);
            memcpy(user_buf_p, sec_buf_p, len_in_loop);
            sec_buf_p = sec_buf;
            user_buf_p += len_in_loop;
            pos += len_in_loop;
            read_len_counter += len_in_loop;
        }
        src_proc->filp_table[fd]->fd_pos = pos;
        return read_len_counter;
    }
}

int do_write(Message* msg){
    // get data from message
    uint32_t fd = msg->mdata_fs_write.fd;
    void* buf = (void*)(msg->mdata_fs_write.buf);
    uint32_t len = msg->mdata_fs_write.len;
    PCB* src_proc = &pcb_table[msg->src_pid];
    uint8_t sec_buf[SECTOR_SIZE];

    // check access mode
    if(!(src_proc->filp_table[fd]->fd_mode & O_RDWR)){
        return -1;
    }

    // prepare for copy
    INode* inode_ptr = src_proc->filp_table[fd]->fd_inode;
    int pos = src_proc->filp_table[fd]->fd_pos;
    int pos_end = pos + len;


    if((inode_ptr->access_mode & ACCESS_MODE_TYPE_MASK) == ACCESS_MODE_CHAR_SPECIAL){
        //todo tty
        return 0;
    }
    else{ // copy to hd sec
        int sec_min = inode_ptr->nr_start_sector + (pos / SECTOR_SIZE);
        int sec_max = inode_ptr->nr_start_sector + (pos_end / SECTOR_SIZE);
        uint8_t* sec_buf_p = sec_buf +  (pos % SECTOR_SIZE);
        uint8_t* user_buf_p = buf;
        int write_len_counter = 0;
        for(int sec = sec_min; sec <= sec_max; sec++){
            read_sector(sec, sec_buf, SECTOR_SIZE);
            int len_in_loop = min(SECTOR_SIZE - (pos % SECTOR_SIZE), len - write_len_counter);
            printString("LEN IN LOOP: ", -1);printInt32(len_in_loop);printString("\n", -1);
            printString("WRITE INTO HD, DATA: ", -1);printString((char*)user_buf_p, len_in_loop);printString("\n", -1);
            memcpy(sec_buf_p, user_buf_p, len_in_loop);
            sec_buf_p = sec_buf;
            user_buf_p += len_in_loop;
            pos += len_in_loop;
            write_len_counter += len_in_loop;
            write_sector(sec, sec_buf, SECTOR_SIZE);
        }
        src_proc->filp_table[fd]->fd_pos = pos;
        if(inode_ptr->file_size < pos){
            inode_ptr->file_size = pos;
        }
        sync_inode(inode_ptr);
        return write_len_counter;
    }
}
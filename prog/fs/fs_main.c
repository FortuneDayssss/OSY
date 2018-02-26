#include "type.h"
#include "fs.h"
#include "print.h"
#include "ipc.h"
#include "message.h"
#include "process.h"
#include "string.h"
#include "global.h"
#include "hd.h"
#include "tty.h"
#include "stdio.h"

void init_fs();
void mkfs();
void load_super_block();
INode* get_inode(uint32_t dev,int nr_inode);

void fs_main(){
    init_fs();

    //--------------test------------------
    Message test_msg;
    char test_path_name[20] = "/test_file_2";
    test_msg.src_pid = 1;
    test_msg.type = MSG_FS_OPEN;
    test_msg.mdata_fs_open.path_name = (uint32_t)test_path_name;
    test_msg.mdata_fs_open.path_name_len = strlen(test_path_name);
    test_msg.mdata_fs_open.flags = O_RDWR;

    printString("test_msg generated!\n", -1);
    int test_fd = do_open(&test_msg);
    printString("test fd: ", -1);printInt32(test_fd);printString("\n", -1);
    printString("test fd's access mode: ", -1);printInt32(pcb_table[1].filp_table[test_fd]->fd_inode->access_mode);printString("\n", -1);
    printString("test fd's start sector: ", -1);printInt32(pcb_table[1].filp_table[test_fd]->fd_inode->nr_start_sector);printString("\n", -1);
    
    //--------------test end--------------

    printString("init fs ok! fs service is waiting for message...\n", -1);
    Message msg;
    while(1){
        sys_ipc_recv(PID_ANY, &msg);
        switch(msg.type){
            case MSG_FS_OPEN:
                msg.mdata_response.fd = do_open(&msg);
                break;
            case MSG_FS_CLOSE:
                break;
            case MSG_FS_READ:
                break;
            case MSG_FS_WRITE:
                break;
            case MSG_FS_UNLINK:
                break;
            case MSG_FS_RESUME_PROC:
                break;
            default:
                break;
        }
    }
}

void init_fs(){
    for(int i = 0; i < NR_FILE_DESCRIPTOR; i++)
        memset(&fd_table[i], 0, sizeof(File_Descriptor));

    for(int i = 0; i < NR_INODE; i++)
        memset(&inode_table[i], 0, sizeof(INode));

    for(int i = 0; i < NR_SUPER_BLOCK; i++){
        memset(&super_block_table[i], 0, sizeof(Super_Block));
        super_block_table[i].dev_no = NO_DEV;
    }


    load_super_block();
    printString("load super block ok!\n", -1);

    Super_Block* sb = get_super_block(MAKE_DEV(DEV_HD, 1));
    if(sb->magic != FS_OSY_MAGIC_V1){
        printString("there is no file system, create a system...\n", -1);
        mkfs();
        printString("mkfs ok!\n", -1);
    }
    else{
        printString("file system was detected!\n", -1);
    }

    
    root_inode = get_inode(MAKE_DEV(DEV_HD, 1) ,ROOT_INODE);
    printString("load root inode ok!\n", -1);
    printString("root inode access mode: ", -1);printInt32(root_inode->access_mode);printString("\n", -1);
}

void read_sector(uint32_t sector, void* buf, uint32_t len){
    Message msg;
    msg.type = MSG_HD_READ;
    msg.mdata_hd_read.sector = sector;
    msg.mdata_hd_read.buf_addr = (uint32_t)buf;
    msg.mdata_hd_read.len = len;
    sys_ipc_send(PID_HD, &msg);
    sys_ipc_recv(PID_HD, &msg);
}

void write_sector(uint32_t sector, void* buf, uint32_t len){
    Message msg;
    msg.type = MSG_HD_WRITE;
    msg.mdata_hd_write.sector = sector;
    msg.mdata_hd_write.buf_addr = (uint32_t)buf;
    msg.mdata_hd_write.len = len;
    printString("fs wirte sector send message\n", -1);
    sys_ipc_send(PID_HD, &msg);
    sys_ipc_recv(PID_HD, &msg);
    printString("fs write sector ok\n", -1);
}

// uint8_t fs_buf[SECTOR_SIZE * 2];
uint8_t* fs_buf = (uint8_t*)(0x00600000);

void mkfs(){
    /*  sectors in hard disk
        0:      boot sector (todo: need migrate)
        1:      super block
        2:      inode bitmap
        3-n:    sector bitmap
        n-m:    inode array
        m-end:  data
    */

    // super block
    Super_Block sb;
    sb.magic = FS_OSY_MAGIC_V1;
    sb.nr_inodes = SECTOR_SIZE * 8;
    sb.nr_inode_sectors = sb.nr_inodes * INODE_SIZE / SECTOR_SIZE + 1;
    sb.nr_sectors = 70 * 1024 * 1024 / 512; //dummy number,  todo:  !!!!!!!!get sector number of hard disk
    sb.nr_imap_sectors = 1;
    sb.nr_smap_sectors = sb.nr_sectors / (8 * SECTOR_SIZE) + 1;
    sb.nr_first_data_sector = 1 + 1 + sb.nr_imap_sectors + sb.nr_smap_sectors + sb.nr_inode_sectors;
    sb.root_inode = ROOT_INODE;
    sb.inode_size = INODE_SIZE;

    INode inode_example;
    sb.inode_file_size_offset = (uint32_t)(&inode_example.file_size) - (uint32_t)(&inode_example);
    sb.inode_start_sector_offset = (uint32_t)(inode_example.nr_start_sector) - (uint32_t)(&inode_example);
    sb.dir_entry_size = DIR_ENTRY_SIZE;

    Dir_Entry dir_entry_example;
    sb.dir_entry_inode_offset = (uint32_t)(&dir_entry_example.nr_inode) - (uint32_t)(&dir_entry_example);
    sb.dir_entry_fname_offset = (uint32_t)(&dir_entry_example.file_name) - (uint32_t)(&dir_entry_example);

    write_sector(1, &sb, 56);
    printString("super block init finish\n", -1);

    // inode bitmap
    /*
        0:          reserved
        1:          root inode
        2:          tty0
        ...
        10:         tty7
        11-end:     available
    */
    memset(fs_buf, 0, SECTOR_SIZE);
    fs_buf[0] = 0xFF;
    fs_buf[1] = 3;
    write_sector(2, &fs_buf[0], SECTOR_SIZE);
    printString("inode bitmap init finish\n", -1);

    // sector bitmap
    memset(fs_buf, 0, SECTOR_SIZE);
    int used_nr_sectors = NR_DEFAULT_FILE_SECTORS + 1;

    int i;
    for(i = 0; i < used_nr_sectors / 8; i++)
        fs_buf[i] = 0xFF;
    for(int j = 0; j < used_nr_sectors % 8; j++)
        fs_buf[i] |= (1 << j);
    write_sector(2 + sb.nr_imap_sectors, fs_buf, SECTOR_SIZE);
    memset(fs_buf, 0, SECTOR_SIZE);
    for(int i = 1; i < sb.nr_smap_sectors; i++){
        printInt32(i);printString(" of ", -1);printInt32(sb.nr_smap_sectors);printString("\n", -1);
        write_sector(2 + sb.nr_imap_sectors + i, fs_buf, SECTOR_SIZE);
    }
    printString("sector bitmap init finish\n", -1);

    // inodes ., tty0, tty1, ..., tty7
    memset(fs_buf, 0, SECTOR_SIZE);
    INode* inode_ptr = (INode*)fs_buf;
    inode_ptr->access_mode = ACCESS_MODE_DIRECTORY;
    inode_ptr->file_size = DIR_ENTRY_SIZE * 9;
    inode_ptr->nr_start_sector = sb.nr_first_data_sector;
    inode_ptr->nr_sectors = NR_DEFAULT_FILE_SECTORS;

    for(int i = 0; i < NR_CONSOLES; i++){
        ((INode*)(&fs_buf[(i + 1) * INODE_SIZE]))->access_mode = ACCESS_MODE_CHAR_SPECIAL;
        ((INode*)(&fs_buf[(i + 1) * INODE_SIZE]))->file_size = 0;
        ((INode*)(&fs_buf[(i + 1) * INODE_SIZE]))->nr_start_sector = MAKE_DEV(DEV_TTY, i);
        ((INode*)(&fs_buf[(i + 1) * INODE_SIZE]))->nr_sectors = 0;
    }
    write_sector(2 + sb.nr_imap_sectors + sb.nr_smap_sectors, fs_buf, SECTOR_SIZE);
    printString("inode init finish\n", -1);

    // root
    memset(fs_buf, 0, SECTOR_SIZE);
    Dir_Entry* root_dir_entry_ptr = (Dir_Entry*)fs_buf;
    root_dir_entry_ptr->nr_inode = 1;
    strcpy((char*)(root_dir_entry_ptr->file_name), ".");

    // todo: implement sprintf and change tty file init code to loop
    root_dir_entry_ptr++;
    root_dir_entry_ptr->nr_inode = 2;
    strcpy((char*)(root_dir_entry_ptr->file_name), "dev_tty0");
    root_dir_entry_ptr++;
    root_dir_entry_ptr->nr_inode = 3;
    strcpy((char*)(root_dir_entry_ptr->file_name), "dev_tty1");
    root_dir_entry_ptr++;
    root_dir_entry_ptr->nr_inode = 4;
    strcpy((char*)(root_dir_entry_ptr->file_name), "dev_tty2");
    root_dir_entry_ptr++;
    root_dir_entry_ptr->nr_inode = 5;
    strcpy((char*)(root_dir_entry_ptr->file_name), "dev_tty3");
    root_dir_entry_ptr++;
    root_dir_entry_ptr->nr_inode = 6;
    strcpy((char*)(root_dir_entry_ptr->file_name), "dev_tty4");
    root_dir_entry_ptr++;
    root_dir_entry_ptr->nr_inode = 7;
    strcpy((char*)(root_dir_entry_ptr->file_name), "dev_tty5");
    root_dir_entry_ptr++;
    root_dir_entry_ptr->nr_inode = 8;
    strcpy((char*)(root_dir_entry_ptr->file_name), "dev_tty6");
    root_dir_entry_ptr++;
    write_sector(sb.nr_first_data_sector, fs_buf, SECTOR_SIZE);
    memset(fs_buf, 0, SECTOR_SIZE);
    root_dir_entry_ptr = (Dir_Entry*)fs_buf;
    root_dir_entry_ptr->nr_inode = 9;
    strcpy((char*)(root_dir_entry_ptr->file_name), "dev_tty7");
    write_sector(sb.nr_first_data_sector + 1, fs_buf, SECTOR_SIZE);
    printString("root directory init finish\n", -1);

}

void load_super_block(){
    memset(fs_buf, 0, SECTOR_SIZE);
    Message msg;
    msg.type = MSG_HD_READ;
    msg.mdata_hd_read.sector = 1;
    msg.mdata_hd_read.buf_addr = (uint32_t)fs_buf;
    msg.mdata_hd_read.len = SECTOR_SIZE;
    sys_ipc_send(PID_HD, &msg);
    sys_ipc_recv(PID_HD, &msg);

    Super_Block* sb_ptr;
    for(int i = 0; i < NR_SUPER_BLOCK; i++)
        if(super_block_table[i].dev_no == NO_DEV)
            sb_ptr = &(super_block_table[i]);
    *(sb_ptr) = *((Super_Block*)fs_buf);
    sb_ptr->dev_no = MAKE_DEV(DEV_HD, 1);
}

Super_Block* get_super_block(uint32_t dev){
    for(int i = 0; i < NR_SUPER_BLOCK; i++)
        if(super_block_table[i].dev_no == dev)
            return &(super_block_table[i]);
    return 0;
}

INode* get_inode(uint32_t dev, int nr_inode){
    INode* inode_ptr = inode_table;
    INode* inode_ptr_for_load = 0;

    // search inode from inode table in memory
    while(inode_ptr < &(inode_table[NR_INODE])){
        if((inode_ptr->process_counter) && (inode_ptr->dev_no == dev) && (inode_ptr->nr_inode == nr_inode)){
            inode_ptr->process_counter++;
            return inode_ptr;
        }
        else if(inode_ptr->dev_no == INODE_INVALID){
            inode_ptr_for_load = inode_ptr;
            break;
        }
        
        inode_ptr++;
    }
    if(!inode_ptr_for_load){
        return 0; // inode table is full, error (todo: report error)
    }

    // not in memory, load inode from hard disk
    debug_log("NOT IN MEMORY!!!!!!!");
    Super_Block* sbp = get_super_block(dev);
    uint8_t buf[SECTOR_SIZE];
    uint32_t nr_block = 1 + 1 + sbp->nr_imap_sectors + sbp->nr_smap_sectors + 
        ((nr_inode - 1) / (SECTOR_SIZE / INODE_SIZE));
    
    read_sector(nr_block, buf, SECTOR_SIZE);

    // printString("BLOCK: ", -1);printInt32(nr_block);printString("\n", -1);
    // printString("DADDRS: ", -1);printInt32(nr_block * 512);printString("\n", -1);
    // printString("DADDR: ", -1);printInt32(nr_block * 512 + (0xA - 1) * 32);printString("\n", -1);

    // printString("ADDR: ", -1);printInt32((uint32_t)buf);printString("\n", -1);
    // copy from buffer (hard disk)
    inode_ptr = (INode*)(buf + (((nr_inode - 1) % (SECTOR_SIZE / INODE_SIZE)) * INODE_SIZE));
    // printString("OFFSET: ", -1);printInt32(((uint32_t)inode_ptr - (uint32_t)buf) / 32);printString(" * inode size\n", -1);
    // printString("NR INODE: ", -1);printInt32(nr_inode);printString("\n", -1);
    // printString("INODE INDEX: ", -1);printInt32(((nr_inode - 1) % (SECTOR_SIZE / INODE_SIZE)));printString("\n", -1);
    // printString("ACCESS MODE: ", -1);printInt32(inode_ptr->access_mode);printString("\n", -1);
    inode_ptr_for_load->access_mode = inode_ptr->access_mode;
    inode_ptr_for_load->file_size = inode_ptr->file_size;
    inode_ptr_for_load->nr_start_sector = inode_ptr->nr_start_sector;
    inode_ptr_for_load->nr_sectors = inode_ptr->nr_sectors;


    // generate data and save in memory
    inode_ptr_for_load->dev_no = dev;
    inode_ptr_for_load->nr_inode = nr_inode;
    inode_ptr_for_load->process_counter = 1;

    return inode_ptr_for_load;
}

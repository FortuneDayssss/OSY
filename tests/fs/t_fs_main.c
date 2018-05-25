#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

/* function && variable rename */
#define printString mock_printString
#define printInt32 mock_printInt32
#define sys_ipc_send mock_sys_ipc_send
#define sys_ipc_recv mock_sys_ipc_recv
#define get_process_phy_mem mock_get_process_phy_mem
#define error_log mock_error_log
#define search_file mock_search_file
#define do_open mock_do_open
#define do_close mock_do_close
#define do_read mock_do_read
#define do_write mock_do_write
#define memset mock_memset

#define pcb_table mock_pcb_table
#define fd_table mock_fd_table
#define inode_table mock_inode_table
#define super_block_table mock_super_block_table
#define root_inode mock_root_inode

#include "../../prog/fs/fs_main.c"

/* variable mock */

PCB                 pcb_table[MAX_PROCESS_NUM];
File_Descriptor     fd_table[NR_FILE_DESCRIPTOR];
INode               inode_table[NR_INODE];
Super_Block         super_block_table[NR_SUPER_BLOCK];
INode*              root_inode;

/* function mock */
int counter_mock_printString = 0;
int counter_mock_printInt32 = 0;
int counter_mock_sys_ipc_recv = 0;
int counter_mock_sys_ipc_send = 0;
int counter_mock_get_process_phy_mem = 0;
int counter_mock_error_log = 0;
int counter_mock_search_file = 0;
int counter_mock_do_open = 0;
int counter_mock_do_close = 0;
int counter_mock_do_read = 0;
int counter_mock_do_write = 0;
int counter_mock_memset = 0;

void mock_printString(char* str, int size){
    counter_mock_printString++;
}
void mock_printInt32(uint32_t n){
    counter_mock_printInt32++;
}
Message mock_recv_msg;
uint32_t mock_sys_ipc_recv(uint32_t src_pid, Message* msg_ptr){
    counter_mock_sys_ipc_recv++;
    memcpy(msg_ptr, &mock_recv_msg, sizeof(Message));
    return 0;
}
Message mock_send_msg;
uint32_t mock_sys_ipc_send(uint32_t dst_pid, Message* msg_ptr){
    memcpy(&mock_send_msg, msg_ptr, sizeof(Message));
    counter_mock_sys_ipc_send++;
    return 0;
}
uint32_t mock_get_process_phy_mem(uint32_t pid, uint32_t addr){
    counter_mock_get_process_phy_mem++;
    return (uint32_t)mock();
}
void mock_error_log(char* log){
    counter_mock_error_log++;
}
int mock_search_file(char* path){
    counter_mock_search_file++;
    return (int)mock();
}
int mock_do_open(Message* msg){
    counter_mock_do_open++;
    return (int)mock();
}
int mock_do_close(Message* msg){
    counter_mock_do_close++;
    return (int)mock();
}
int mock_do_read(Message* msg){
    counter_mock_do_read++;
    return (int)mock();
}
int mock_do_write(Message* msg){
    counter_mock_do_write++;
    return (int)mock();
}
void* mock_memset(void * dst, char ch, int size){
    counter_mock_memset++;
    return 0;
}

/* unit test */
static void test_init_fs(void **state){
    init_fs();
    assert_int_equal(root_inode->nr_inode, 1);
}

static void test_do_fork_fd(void **state){
    Message msg;
    msg.mdata_fs_fork_fd.pid = 1;

    INode inode;
    inode.process_counter = 1;

    File_Descriptor fd;
    fd.process_counter = 1;
    fd.fd_inode = &inode;
    
    
    pcb_table[1].filp_table[0] = 0;
    pcb_table[1].filp_table[1] = &fd;
    
    do_fork_fd(&msg);
    assert_ptr_equal(pcb_table[1].filp_table[0], 0);
    assert_int_equal(inode.process_counter, 2);
    assert_int_equal(fd.process_counter, 2);
}

static void test_do_fs_exit(void **state){
    Message msg;
    msg.mdata_fs_fork_fd.pid = 1;

    INode inode;
    inode.process_counter = 1;

    File_Descriptor fd;
    fd.process_counter = 1;
    fd.fd_inode = &inode;
    
    
    pcb_table[1].filp_table[0] = 0;
    pcb_table[1].filp_table[1] = &fd;
    
    do_fs_exit(&msg);
    assert_ptr_equal(pcb_table[1].filp_table[0], 0);
    assert_int_equal(inode.process_counter, 0);
    assert_int_equal(fd.process_counter, 0);
}

static void test_do_fs_stat_file_not_exist(void **state){
    Message msg;
    msg.mdata_fs_fork_fd.pid = 1;
    File_Stat file_stat;
    will_return_always(mock_get_process_phy_mem, &file_stat);
    will_return_always(mock_search_file, INODE_INVALID);
    
    do_fs_stat(&msg);
    assert_int_equal(mock_send_msg.mdata_response.status, RESPONSE_FAIL);
}
static void test_do_fs_stat_file_exist(void **state){
    Message msg;
    msg.mdata_fs_fork_fd.pid = 1;
    File_Stat file_stat;
    inode_table[1].access_mode = ACCESS_MODE_REGULAR;
    inode_table[1].dev_no = MAKE_DEV(DEV_HD, 1);
    inode_table[1].process_counter = 1;
    inode_table[1].nr_start_sector = 233;
    
    will_return_always(mock_get_process_phy_mem, &file_stat);
    will_return_always(mock_search_file, 1);
    
    do_fs_stat(&msg);
    assert_int_equal(mock_send_msg.mdata_response.status, RESPONSE_SUCCESS);
}

int main(int argc, char* argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_init_fs),
        cmocka_unit_test(test_do_fork_fd),
        cmocka_unit_test(test_do_fs_exit),
        cmocka_unit_test(test_do_fs_stat_file_not_exist),
        cmocka_unit_test(test_do_fs_stat_file_exist)
};

    return cmocka_run_group_tests(tests, NULL, NULL);
}

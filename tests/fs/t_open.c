#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

/* function && variable rename */
#define printString mock_printString
#define printInt32 mock_printInt32
#define debug_log mock_debug_log
#define error_log mock_error_log
#define get_process_phy_mem mock_get_process_phy_mem
#define search_file mock_search_file
#define read_sector mock_read_sector
#define write_sector mock_write_sector
#define get_inode mock_get_inode
#define put_inode mock_put_inode
#define get_super_block mock_get_super_block

#include "../../prog/fs/open.c"

/* variable mock */
PCB pcb_table[MAX_PROCESS_NUM];
File_Descriptor fd_table[NR_FILE_DESCRIPTOR];

/* function mock */
int counter_mock_printString = 0;
int counter_mock_printInt32 = 0;
int counter_mock_debug_log = 0;
int counter_mock_error_log = 0;
int counter_mock_get_process_phy_mem = 0;
int counter_mock_search_file = 0;
int counter_mock_read_sector = 0;
int counter_mock_write_sector = 0;
int counter_mock_get_inode = 0;
int counter_mock_put_inode = 0;
int counter_mock_get_super_block = 0;


void mock_printString(char* str, int size){
    counter_mock_printString++;
}
void mock_printInt32(uint32_t n){
    counter_mock_printInt32++;
}
void mock_debug_log(char* log){
    counter_mock_debug_log++;
}
void mock_error_log(char* log){
    counter_mock_error_log++;
}
int mock_search_file(char* path){
    counter_mock_search_file++;
    return (int)mock();
}
uint32_t mock_get_process_phy_mem(uint32_t pid, uint32_t addr){
    counter_mock_get_process_phy_mem++;
    return (uint32_t)mock();
}
void mock_read_sector(uint32_t sector, void* buf, uint32_t len){
    counter_mock_read_sector++;
}
void mock_write_sector(uint32_t sector, void* buf, uint32_t len){
    counter_mock_write_sector++;
}
INode* mock_get_inode(uint32_t dev, int nr_inode){
    counter_mock_get_inode++;
    return (INode*)(unsigned int)mock();
}
INode* mock_put_inode(INode* inode_ptr){
    counter_mock_put_inode++;
    return (INode*)(unsigned int)mock();
}
Super_Block* mock_get_super_block(uint32_t dev){
    counter_mock_get_super_block++;
    return (Super_Block*)(unsigned int)mock();
}

/* unit test */
static void test_do_open_out_of_path_len(void **state){
    Message msg;
    msg.mdata_fs_open.path_name_len = 9999;
    assert_int_equal(do_open(&msg), -1);
}
static void test_do_open_filp_table_is_full(void **state){
    char path[20];
    path[0] = '/';
    path[1] = 'a';
    path[2] = '\0';
    Message msg;
    msg.mdata_fs_open.path_name_len = 2;
    msg.mdata_fs_open.path_name = (uint32_t)path;
    msg.src_pid = 1;
    will_return(mock_get_process_phy_mem, path);

    for(int i = 0; i < FILP_TABLE_SIZE; i++){
        pcb_table[1].filp_table[i] = (File_Descriptor*)1;
    }

    assert_int_equal(do_open(&msg), -1);
    
    for(int i = 0; i < FILP_TABLE_SIZE; i++){
        pcb_table[1].filp_table[i] = 0;
    }
}

static void test_do_open_fd_table_is_full(void **state){
    char path[20];
    path[0] = '/';
    path[1] = 'a';
    path[2] = '\0';
    Message msg;
    msg.mdata_fs_open.path_name_len = 2;
    msg.mdata_fs_open.path_name = (uint32_t)path;
    msg.src_pid = 1;
    will_return(mock_get_process_phy_mem, path);

    for(int i = 0; i < NR_FILE_DESCRIPTOR; i++){
        fd_table[i].fd_inode = (INode*)1;
    }

    assert_int_equal(do_open(&msg), -1);
    
    for(int i = 0; i < NR_FILE_DESCRIPTOR; i++){
        fd_table[i].fd_inode = 0;
    }
}

static void test_do_open_create_mode_file_exist(void **state){
    char path[20];
    path[0] = '/';
    path[1] = 'a';
    path[2] = '\0';
    Message msg;
    msg.mdata_fs_open.path_name_len = 2;
    msg.mdata_fs_open.path_name = (uint32_t)path;
    msg.mdata_fs_open.flags = O_CREATE;
    msg.src_pid = 1;
    will_return(mock_get_process_phy_mem, path);
    will_return(mock_search_file, 1);
    assert_int_equal(do_open(&msg), -1);
}

static void test_do_open_create_mode_success(void **state){
    char path[20];
    path[0] = '/';
    path[1] = 'a';
    path[2] = '\0';
    Message msg;
    msg.mdata_fs_open.path_name_len = 2;
    msg.mdata_fs_open.path_name = (uint32_t)path;
    msg.mdata_fs_open.flags = O_CREATE;
    msg.src_pid = 1;
    INode new_inode;
    Super_Block super_block;
    will_return(mock_get_process_phy_mem, path);
    will_return_always(mock_search_file, INODE_INVALID);
    will_return_always(mock_get_inode, &new_inode);
    will_return_always(mock_get_super_block, &super_block);

    assert_int_equal(do_open(&msg), 0);
}

static void test_do_open_error_mode(void **state){
    char path[20];
    path[0] = '/';
    path[1] = 'a';
    path[2] = '\0';
    Message msg;
    msg.mdata_fs_open.path_name_len = 2;
    msg.mdata_fs_open.path_name = (uint32_t)path;
    msg.mdata_fs_open.flags = 0;
    msg.src_pid = 1;
    INode new_inode;
    Super_Block super_block;
    will_return(mock_get_process_phy_mem, path);
    will_return_always(mock_search_file, INODE_INVALID);

    assert_int_equal(do_open(&msg), -1);
}

static void test_do_open_rw_mode_file_not_exist(void **state){
    char path[20];
    path[0] = '/';
    path[1] = 'a';
    path[2] = '\0';
    Message msg;
    msg.mdata_fs_open.path_name_len = 2;
    msg.mdata_fs_open.path_name = (uint32_t)path;
    msg.mdata_fs_open.flags = O_RDWR;
    msg.src_pid = 1;
    INode new_inode;
    Super_Block super_block;
    will_return(mock_get_process_phy_mem, path);
    will_return_always(mock_search_file, INODE_INVALID);

    assert_int_equal(do_open(&msg), -1);
}

static void test_do_open_rw_mode_file_inode_lost(void **state){
    char path[20];
    path[0] = '/';
    path[1] = 'a';
    path[2] = '\0';
    Message msg;
    msg.mdata_fs_open.path_name_len = 2;
    msg.mdata_fs_open.path_name = (uint32_t)path;
    msg.mdata_fs_open.flags = O_RDWR;
    msg.src_pid = 1;
    Super_Block super_block;
    will_return(mock_get_process_phy_mem, path);
    will_return_always(mock_search_file, 2);
    will_return_always(mock_get_inode, 0);

    assert_int_equal(do_open(&msg), -1);
}

static void test_do_open_rw_mode_file_success(void **state){
    char path[20];
    path[0] = '/';
    path[1] = 'a';
    path[2] = '\0';
    Message msg;
    msg.mdata_fs_open.path_name_len = 2;
    msg.mdata_fs_open.path_name = (uint32_t)path;
    msg.mdata_fs_open.flags = O_RDWR;
    msg.src_pid = 1;
    Super_Block super_block;
    INode new_inode;
    will_return(mock_get_process_phy_mem, path);
    will_return_always(mock_search_file, 2);
    will_return_always(mock_get_inode, &new_inode);

    assert_int_equal(do_open(&msg), -1);
}

static void test_do_close(void **state){
    Message msg;
    msg.mdata_fs_close.fd = 1;
    msg.src_pid = 1;
    File_Descriptor file_descriptor;
    INode dummy_inode;
    pcb_table[1].filp_table[1] = &file_descriptor;
    pcb_table[1].filp_table[1]->process_counter = 1;
    pcb_table[1].filp_table[1]->fd_inode = &dummy_inode;
    will_return(mock_put_inode, 0);

    assert_int_equal(do_close(&msg), 0);
    assert_ptr_equal(pcb_table[1].filp_table[1], 0);
    assert_int_equal(file_descriptor.process_counter, 0);
    assert_ptr_equal(file_descriptor.fd_inode, 0);
}

int main(int argc, char* argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_do_open_out_of_path_len),
        cmocka_unit_test(test_do_open_filp_table_is_full),
        cmocka_unit_test(test_do_open_fd_table_is_full),
        cmocka_unit_test(test_do_open_create_mode_file_exist),
        cmocka_unit_test(test_do_open_create_mode_success),
        cmocka_unit_test(test_do_open_error_mode),
        cmocka_unit_test(test_do_open_rw_mode_file_not_exist),
        cmocka_unit_test(test_do_open_rw_mode_file_inode_lost),
        cmocka_unit_test(test_do_close)
        
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

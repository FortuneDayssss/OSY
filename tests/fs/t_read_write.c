#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

/* function && variable rename */
#define error_log mock_error_log
#define sys_ipc_send mock_sys_ipc_send
#define sys_ipc_recv mock_sys_ipc_recv
#define get_process_phy_mem mock_get_process_phy_mem
#define read_sector mock_read_sector
#define write_sector mock_write_sector
#define sync_inode mock_sync_inode

#include "../../prog/fs/read_write.c"

/* variable mock */
PCB pcb_table[MAX_PROCESS_NUM];


/* function mock */
int counter_mock_error_log = 0;
int counter_mock_sys_ipc_recv = 0;
int counter_mock_sys_ipc_send = 0;
int counter_mock_get_process_phy_mem = 0;
int counter_mock_read_sector = 0;
int counter_mock_write_sector = 0;
int counter_mock_sync_inode = 0;


void mock_error_log(char* log){
    counter_mock_error_log++;
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
void read_sector(uint32_t sector, void* buf, uint32_t len){
    counter_mock_read_sector++;
}
void write_sector(uint32_t sector, void* buf, uint32_t len){
    counter_mock_write_sector++;
}
void* mock_sync_inode(INode* inode_ptr){
    counter_mock_sync_inode++;
}

/* unit test */
static void test_do_read_fd_access_mode_error(void **state){
    Message msg;
    char buf[20];
    msg.src_pid = 1;
    msg.mdata_fs_read.fd = 1;
    msg.mdata_fs_read.buf = (uint32_t)buf;
    msg.mdata_fs_read.len = 10;
    
    File_Descriptor file_descriptor;
    pcb_table[1].filp_table[1] = &file_descriptor;
    file_descriptor.fd_mode = 0;

    assert_int_equal(do_read(&msg), -1);
}

static void test_do_read_tty_hook_fail(void **state){
    Message msg;
    char buf[20];
    msg.src_pid = 1;
    msg.mdata_fs_read.fd = 1;
    msg.mdata_fs_read.buf = (uint32_t)buf;
    msg.mdata_fs_read.len = 10;
    
    File_Descriptor file_descriptor;
    INode inode;
    pcb_table[1].filp_table[1] = &file_descriptor;
    file_descriptor.fd_mode = O_RDWR;
    file_descriptor.fd_inode = &inode;
    inode.access_mode = ACCESS_MODE_CHAR_SPECIAL;

    counter_mock_sys_ipc_recv = 0;
    counter_mock_sys_ipc_send = 0;

    assert_int_equal(do_read(&msg), 0);
    assert_int_equal(counter_mock_sys_ipc_recv, 1);
    assert_int_equal(counter_mock_sys_ipc_send, 2);
}

static void test_do_read_hd(void **state){
    Message msg;
    char buf[20];
    msg.src_pid = 1;
    msg.mdata_fs_read.fd = 1;
    msg.mdata_fs_read.buf = (uint32_t)buf;
    msg.mdata_fs_read.len = 10;
    
    File_Descriptor file_descriptor;
    INode inode;
    pcb_table[1].filp_table[1] = &file_descriptor;
    file_descriptor.fd_mode = O_RDWR;
    file_descriptor.fd_inode = &inode;
    inode.access_mode = ACCESS_MODE_REGULAR;

    assert_int_equal(do_read(&msg), 0);
}

static void test_do_write_fd_access_mode_error(void **state){
    Message msg;
    char buf[20];
    msg.src_pid = 1;
    msg.mdata_fs_write.fd = 1;
    msg.mdata_fs_write.buf = (uint32_t)buf;
    msg.mdata_fs_write.len = 10;
    
    File_Descriptor file_descriptor;
    pcb_table[1].filp_table[1] = &file_descriptor;
    file_descriptor.fd_mode = 0;

    assert_int_equal(do_write(&msg), -1);
}

static void test_do_write_tty(void **state){
    Message msg;
    char buf[20];
    buf[0] = 'a';
    msg.src_pid = 1;
    msg.mdata_fs_write.fd = 1;
    msg.mdata_fs_write.buf = (uint32_t)buf;
    msg.mdata_fs_write.len = 1;
    
    File_Descriptor file_descriptor;
    INode inode;
    pcb_table[1].filp_table[1] = &file_descriptor;
    file_descriptor.fd_mode = O_RDWR;
    file_descriptor.fd_inode = &inode;
    inode.access_mode = ACCESS_MODE_CHAR_SPECIAL;
    mock_recv_msg.mdata_response.len = 5;

    counter_mock_sys_ipc_recv = 0;
    counter_mock_sys_ipc_send = 0;

    assert_int_equal(do_write(&msg), 5);
    assert_int_equal(counter_mock_sys_ipc_recv, 1);
    assert_int_equal(counter_mock_sys_ipc_send, 1);
}
static void test_do_write_hd(void **state){
    Message msg;
    char buf[20];
    msg.src_pid = 1;
    msg.mdata_fs_write.fd = 1;
    msg.mdata_fs_write.buf = (uint32_t)buf;
    msg.mdata_fs_write.len = 10;
    
    File_Descriptor file_descriptor;
    INode inode;
    pcb_table[1].filp_table[1] = &file_descriptor;
    file_descriptor.fd_mode = O_RDWR;
    file_descriptor.fd_inode = &inode;
    inode.access_mode = ACCESS_MODE_REGULAR;

    will_return(mock_get_process_phy_mem, buf);

    assert_int_equal(do_write(&msg), 10);
}

int main(int argc, char* argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_do_read_fd_access_mode_error),
        cmocka_unit_test(test_do_read_tty_hook_fail),
        cmocka_unit_test(test_do_read_hd),
        cmocka_unit_test(test_do_write_fd_access_mode_error),
        cmocka_unit_test(test_do_write_tty),
        cmocka_unit_test(test_do_write_hd),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

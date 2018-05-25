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
#define get_desc_base mock_get_desc_base
#define init_descriptor mock_init_descriptor
#define get_process_vir_mem mock_get_process_vir_mem
#define memcpy mock_memcpy
#define memset mock_memset
#define stat mock_stat
#define open mock_open
#define read mock_read
#define close mock_close


#include "../../prog/mm/mm_main.c"

/* variable mock */
PCB                 pcb_table[MAX_PROCESS_NUM];

/* function mock */
int counter_mock_printString = 0;
int counter_mock_printInt32 = 0;
int counter_mock_sys_ipc_recv = 0;
int counter_mock_sys_ipc_send = 0;
int counter_mock_get_process_phy_mem = 0;
int counter_mock_error_log = 0;
int counter_mock_get_desc_base = 0;
int counter_mock_init_descriptor = 0;
int counter_mock_get_process_vir_mem = 0;
int counter_mock_memcpy = 0;
int counter_mock_memset = 0;
int counter_mock_stat = 0;
int counter_mock_open = 0;
int counter_mock_read = 0;
int counter_mock_close = 0;


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
uint32_t mock_get_desc_base(Descriptor* descriptor){
    counter_mock_get_desc_base++;
    return (uint32_t)mock();
}
void mock_init_descriptor(Descriptor* descriptor, uint32_t base, uint32_t limit, uint16_t attribute){
    counter_mock_init_descriptor++;
}
uint32_t mock_get_process_vir_mem(uint32_t pid, uint32_t phy_addr){
    counter_mock_get_process_vir_mem++;
    return (uint32_t)mock();
}
void* mock_memcpy(void* dst, void* src, int size){
    counter_mock_memcpy++;
    return 0;
}
void* mock_memset(void* dst, char ch, int size){
    counter_mock_memset++;
    return 0;
}
File_Stat mock_file_stat;
int mock_stat(const char* path, File_Stat* file_stat){
    counter_mock_stat++;
    *file_stat = mock_file_stat;
    return (int)mock();
}
int mock_open(const char* path_name, int flags){
    counter_mock_open++;
    return (int)mock();
}
char mock_read_buffer[1024];
int mock_read(int fd, void* buf, int count){
    char* p = (char*)buf;
    for(int i = 0; i < count; i++){
        p[i] = mock_read_buffer[i];
    }
    counter_mock_read++;
    return (int)mock();
}
int mock_close(int fd){
    counter_mock_close++;
    return (int)mock();
}

/* unit test */

static void test_do_fork(void **state){
    Message msg;
    msg.src_pid = 0;
    pcb_table[0].state = PROCESS_RUNNING;
    pcb_table[1].state = PROCESS_EMPTY;
    char buf[1024];
    will_return(mock_get_desc_base, buf);
    
    assert_int_equal(do_fork(&msg), 1);
    pcb_table[0].state = PROCESS_EMPTY;
    pcb_table[1].state = PROCESS_EMPTY;
}

static void test_exit_parent_waiting(void **state){
    Message msg;
    msg.mdata_mm_exit.status = 1;
    msg.src_pid = 1;
    pcb_table[1].parent = 0;
    pcb_table[0].ipc_flag = IPC_FLAG_WAITING;

    do_exit(&msg);
    assert_int_equal(pcb_table[0].ipc_flag, 0);
}

static void test_exit_parent_not_waiting(void **state){
    Message msg;
    msg.mdata_mm_exit.status = 1;
    msg.src_pid = 1;
    pcb_table[1].parent = 0;
    pcb_table[0].ipc_flag = 0;

    do_exit(&msg);
    assert_int_equal(pcb_table[0].ipc_flag, 0);
    assert_int_equal(pcb_table[1].ipc_flag, IPC_FLAG_HANGING);
}

static void test_wait_success(void **state){
    Message msg;
    msg.src_pid = 0;
    pcb_table[1].parent = 0;
    pcb_table[1].ipc_flag = IPC_FLAG_HANGING;

    do_wait(&msg);
    assert_int_equal(pcb_table[0].ipc_flag, 0);
    assert_int_equal(pcb_table[1].ipc_flag, 0);
}

static void test_wait_block(void **state){
    Message msg;
    msg.src_pid = 0;
    pcb_table[1].parent = 0;
    pcb_table[1].ipc_flag = 0;

    do_wait(&msg);
    assert_int_equal(pcb_table[0].ipc_flag, IPC_FLAG_WAITING);
    assert_int_equal(pcb_table[1].ipc_flag, 0);
}

static void test_exec_stat_fail(void **state){
    Message msg;
    msg.mdata_mm_exec.args_buf = 0;
    msg.mdata_mm_exec.args_buf_len = 0;
    msg.mdata_mm_exec.path_name = 0;
    msg.mdata_mm_exec.path_name_len = 0;    
    msg.src_pid = 0;

    will_return_always(mock_get_process_phy_mem, 0);
    will_return_always(mock_stat, RESPONSE_FAIL);    

    assert_int_equal(do_exec(&msg), 0);
    assert_int_equal(pcb_table[0].state, PROCESS_EMPTY);
}

static void test_exec_open_fail(void **state){
    Message msg;
    msg.mdata_mm_exec.args_buf = 0;
    msg.mdata_mm_exec.args_buf_len = 0;
    msg.mdata_mm_exec.path_name = 0;
    msg.mdata_mm_exec.path_name_len = 0;    
    msg.src_pid = 0;

    will_return_always(mock_get_process_phy_mem, 0);
    will_return_always(mock_stat, RESPONSE_SUCCESS);    
    will_return_always(mock_open, -1);    

    assert_int_equal(do_exec(&msg), 0);
    assert_int_equal(pcb_table[0].state, PROCESS_EMPTY);
}

static void test_exec_success(void **state){
    Message msg;
    msg.mdata_mm_exec.args_buf = 0;
    msg.mdata_mm_exec.args_buf_len = 1;
    msg.mdata_mm_exec.path_name = 0;
    msg.mdata_mm_exec.path_name_len = 0;
    msg.src_pid = 0;

    Elf32_Ehdr elf32_header_buf;
    mm_elf_buffer = (uint8_t*)(&elf32_header_buf);
    mock_file_stat.size = 50;
    elf32_header_buf.e_phnum = 0;
    char mem_buf[100];

    will_return_always(mock_get_process_phy_mem, mem_buf);
    will_return_always(mock_get_process_vir_mem, mem_buf);
    will_return_always(mock_stat, RESPONSE_SUCCESS);    
    will_return_always(mock_open, 1);
    will_return_always(mock_read, 1);
    will_return_always(mock_close, 1);
    



    assert_int_equal(do_exec(&msg), 1);
}


int main(int argc, char* argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_do_fork),
        cmocka_unit_test(test_exit_parent_waiting),
        cmocka_unit_test(test_exit_parent_not_waiting),
        cmocka_unit_test(test_wait_success),
        cmocka_unit_test(test_wait_block),
        cmocka_unit_test(test_exec_stat_fail),
        cmocka_unit_test(test_exec_open_fail),
        cmocka_unit_test(test_exec_success)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

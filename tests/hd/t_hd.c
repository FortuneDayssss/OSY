#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "type.h"
/* function && variable rename */

#define printString mock_printString
#define printInt32 mock_printInt32
#define printInt8 mock_printInt8
#define sys_ipc_send mock_sys_ipc_send
#define sys_ipc_recv mock_sys_ipc_recv
#define get_process_phy_mem mock_get_process_phy_mem
#define error_log mock_error_log
#define out_byte mock_out_byte
#define in_byte mock_in_byte
#define port_read_16 mock_port_read_16
#define port_write_16 mock_port_write_16
#define sys_ipc_int_send mock_sys_ipc_int_send
#define set_irq_handler mock_set_irq_handler

#include "../../prog/hd/hd.c"

/* variable mock */

/* function mock */
int counter_mock_printString = 0;
int counter_mock_printInt32 = 0;
int counter_mock_printInt8 = 0;
int counter_mock_sys_ipc_recv = 0;
int counter_mock_sys_ipc_send = 0;
int counter_mock_get_process_phy_mem = 0;
int counter_mock_error_log = 0;
int counter_mock_out_byte = 0;
int counter_mock_in_byte = 0;
int counter_mock_port_read_16 = 0;
int counter_mock_port_write_16 = 0;
int counter_mock_sys_ipc_int_send = 0;
int counter_mock_set_irq_handler = 0;

void mock_printString(char* str, int size){
    counter_mock_printString++;
}
void mock_printInt32(uint32_t n){
    counter_mock_printInt32++;
}
void mock_printInt8(uint8_t n){
    counter_mock_printInt8++;
}
Message mock_recv_msg;
uint32_t mock_sys_ipc_recv(uint32_t src_pid, Message* msg_ptr){
    counter_mock_sys_ipc_recv++;
    if(src_pid != PID_INT)
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
void mock_out_byte(uint16_t port, uint8_t value){
    counter_mock_out_byte++;
}
uint8_t mock_in_byte(uint16_t port){
    counter_mock_in_byte++;
    return (uint8_t)mock();
}
void mock_port_read_16(uint16_t port, void* buf, int size){
    counter_mock_port_read_16++;
}
void mock_port_write_16(uint16_t port, void* buf, int size){
    counter_mock_port_write_16++;
}
uint32_t mock_sys_ipc_int_send(uint32_t dst_pid){
    counter_mock_sys_ipc_int_send++;
    return (uint32_t)mock();
}
void mock_set_irq_handler(int vec_no, irq_handler handler){
    counter_mock_set_irq_handler++;
}

/* unit test */
static void test_hd_handler(void **state){
    counter_mock_sys_ipc_int_send = 0;
    will_return(mock_in_byte, 1);
    will_return_always(mock_sys_ipc_int_send, 1);
    hd_handler();
    assert_int_equal(counter_mock_sys_ipc_int_send, 1);
}

static void test_hd_identify(void **state){
    counter_mock_in_byte = 0;
    counter_mock_out_byte = 0;
    counter_mock_port_read_16 = 0;
    
    will_return_always(mock_in_byte, 1);
    hd_identify(0);
    assert_int_equal(counter_mock_in_byte, 2);
    assert_int_equal(counter_mock_out_byte, 8);
    assert_int_equal(counter_mock_port_read_16, 1);
}

static void test_hd_read(void **state){
    Message msg;
    char buf[512];
    msg.mdata_hd_read.buf_addr = (uint32_t)buf;
    msg.mdata_hd_read.len = 20;
    msg.mdata_hd_read.sector = 1;
    will_return_always(mock_in_byte, 1);
    will_return_always(mock_get_process_phy_mem, buf);
    counter_mock_sys_ipc_send = 0;
    hd_read(&msg);
    assert_int_equal(counter_mock_sys_ipc_send, 1);
}

static void test_hd_write(void **state){
    Message msg;
    char buf[512];
    msg.mdata_hd_write.buf_addr = (uint32_t)buf;
    msg.mdata_hd_read.len = 20;
    msg.mdata_hd_read.sector = 1;
    will_return_always(mock_in_byte, 1);
    will_return_always(mock_get_process_phy_mem, buf);
    counter_mock_sys_ipc_send = 0;
    hd_write(&msg);
    assert_int_equal(counter_mock_sys_ipc_send, 1);
}


int main(int argc, char* argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_hd_handler),
        cmocka_unit_test(test_hd_identify),
        cmocka_unit_test(test_hd_read),
        cmocka_unit_test(test_hd_write)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

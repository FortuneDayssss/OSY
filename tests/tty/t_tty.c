#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

/* function && variable rename */
#define keyboard_process mock_keyboard_process
#define debug_print_process_info mock_debug_print_process_info
#define sys_ipc_recv mock_sys_ipc_recv
#define sys_ipc_send mock_sys_ipc_send
#define get_process_phy_mem mock_get_process_phy_mem
#define out_byte mock_out_byte
#define screen_out_char mock_screen_out_char 
#define screen_roll_up mock_screen_roll_up 
#define screen_set_cursor mock_screen_set_cursor 
#define screen_set_base_addr mock_screen_set_base_addr 
#define screen_flush mock_screen_flush 

#define tty_table mock_tty_table
#define current_tty mock_current_tty
#define current_process mock_current_process 

#include "../../prog/tty/tty.c"

/* variable mock */

TTY     mock_tty_table[NR_TTYS];
TTY*    mock_current_tty;
PCB*    mock_current_process;

/* function mock */

int counter_mock_keyboard_process = 0;
int counter_mock_debug_print_process_info = 0;
int counter_mock_sys_ipc_recv = 0;
int counter_mock_sys_ipc_send = 0;
int counter_mock_get_process_phy_mem = 0;
int counter_mock_out_byte = 0;
int counter_mock_screen_out_char = 0;
int counter_mock_screen_roll_up = 0;
int counter_mock_screen_set_cursor = 0;
int counter_mock_screen_set_base_addr = 0;
int counter_mock_screen_flush = 0;


uint32_t mock_key_ret;
uint32_t mock_shift_ret;
uint32_t mock_ctrl_ret;
uint32_t mock_alt_ret;
uint32_t mock_make_ret;
uint32_t have_input_counter;
int mock_keyboard_process(TTY* tty, uint32_t* key_ret, uint32_t* shift_ret, uint32_t* ctrl_ret, uint32_t* alt_ret, uint32_t* make_ret){
    *key_ret = mock_key_ret;
    *shift_ret = mock_shift_ret;
    *ctrl_ret = mock_ctrl_ret;
    *alt_ret = mock_alt_ret;
    *make_ret = mock_make_ret;
    counter_mock_keyboard_process++;
    if(have_input_counter){
        have_input_counter--;
        return (int)mock();
    }
    else{
        return 0;
    }
}
uint32_t mock_debug_print_process_info(uint32_t pid){
    counter_mock_debug_print_process_info++;
    return (uint32_t)mock();
}
uint32_t mock_sys_ipc_recv(uint32_t src_pid, Message* msg_ptr){
    counter_mock_sys_ipc_recv++;
    return (uint32_t)mock();
}
uint32_t mock_sys_ipc_send(uint32_t dst_pid, Message* msg_ptr){
    counter_mock_sys_ipc_send++;
    return (uint32_t)mock();
}
uint32_t mock_get_process_phy_mem(uint32_t pid, uint32_t addr){
    counter_mock_get_process_phy_mem++;
    return (uint32_t)mock();
}
void mock_out_byte(uint16_t port, uint8_t value){
    counter_mock_out_byte++;
}
void mock_screen_out_char(int nr_tty, char ch){
    counter_mock_screen_out_char++;
}
void mock_screen_roll_up(int nr_tty){
    counter_mock_screen_roll_up++;
}
void mock_screen_set_cursor(uint32_t pos){
    counter_mock_screen_set_cursor++;
}
void mock_screen_set_base_addr(uint32_t addr){
    counter_mock_screen_set_base_addr++;
}
void mock_screen_flush(TTY_Console *pc){
    counter_mock_screen_flush++;
}

/* unit test */

static void test_switch_tty(void **state){
    current_tty = &tty_table[1];
    switch_tty(-1);
    assert_ptr_equal(current_tty, &tty_table[1]);
    switch_tty(10);
    assert_ptr_equal(current_tty, &tty_table[1]);
    switch_tty(3);
    assert_ptr_equal(current_tty, &tty_table[3]);
}

static void test_tty_init(void **state){
    TTY* test_tty_ptr = &tty_table[0];
    test_tty_ptr->keyBuffer.count = 0;
    test_tty_ptr->keyBuffer.head = (uint8_t*)1;
    test_tty_ptr->keyBuffer.tail = (uint8_t*)1;
    test_tty_ptr->console.graphMemoryBase = 1;
    test_tty_ptr->console.dispAddr = 1;
    test_tty_ptr->console.cursorAddr = 1;

    tty_init(test_tty_ptr);

    assert_int_equal(test_tty_ptr->keyBuffer.count, 0);
    assert_ptr_equal(test_tty_ptr->keyBuffer.head, test_tty_ptr->keyBuffer.buffer);
    assert_ptr_equal(test_tty_ptr->keyBuffer.tail, test_tty_ptr->keyBuffer.buffer);
    assert_int_equal(test_tty_ptr->console.graphMemoryBase, 0);
    assert_int_equal(test_tty_ptr->console.dispAddr, 0);
    assert_int_equal(test_tty_ptr->console.cursorAddr, 80*24*2);
    
}

static void test_tty_read_loop_without_input(void **state){
    have_input_counter = 1;
    will_return(mock_keyboard_process, 0);
    counter_mock_sys_ipc_send = 0;
    tty_read_loop(&tty_table[0]);
    assert_int_equal(counter_mock_sys_ipc_send, 0);
}

static void test_tty_read_loop_switch_tty_cmd(void **state){
    mock_key_ret = '2';
    mock_shift_ret = 0;
    mock_ctrl_ret = 1;
    mock_alt_ret = 0;
    mock_make_ret = 1;
    current_tty = &tty_table[0];
    have_input_counter = 1;
    will_return(mock_keyboard_process, 1);
    tty_read_loop(&tty_table[0]);
    assert_ptr_equal(current_tty, &tty_table[1]);
}

static void test_tty_read_loop_debug_cmd(void **state){
    mock_key_ret = '2';
    mock_shift_ret = 0;
    mock_ctrl_ret = 0;
    mock_alt_ret = 1;
    mock_make_ret = 1;
    counter_mock_debug_print_process_info = 0;
    have_input_counter = 1;
    will_return(mock_keyboard_process, 1);
    will_return(mock_debug_print_process_info, 1);
    tty_read_loop(&tty_table[0]);
    assert_int_equal(counter_mock_debug_print_process_info, 1);
}

static void test_tty_read_loop_enter(void **state){
    mock_key_ret = ENTER;
    mock_shift_ret = 0;
    mock_ctrl_ret = 0;
    mock_alt_ret = 0;
    mock_make_ret = 1;
    tty_table[0].have_hooked_proc = 1;
    counter_mock_sys_ipc_send = 0;
    counter_mock_screen_roll_up = 0;
    have_input_counter = 1;
    will_return(mock_keyboard_process, 1);
    will_return(mock_sys_ipc_send, 1);
    tty_read_loop(&tty_table[0]);
    assert_int_equal(counter_mock_sys_ipc_send, 1);
    assert_int_equal(counter_mock_screen_roll_up, 1);
}

static void test_tty_read_loop_regular_key(void **state){
    char buf[10];
    mock_key_ret = 'a';
    mock_shift_ret = 0;
    mock_ctrl_ret = 0;
    mock_alt_ret = 0;
    mock_make_ret = 1;
    tty_table[0].have_hooked_proc = 1;
    tty_table[0].proc_buf = buf;
    tty_table[0].copied_len = 0;
    counter_mock_sys_ipc_send = 0;
    have_input_counter = 1;
    will_return(mock_keyboard_process, 1);
    will_return(mock_sys_ipc_send, 1);
    tty_read_loop(&tty_table[0]);
    assert_int_equal(counter_mock_sys_ipc_send, 1);
    assert_int_equal((int)buf[0], (int)'a');
}

static void test_tty_write_loop(void **state){
    tty_table[0].keyBuffer.count = 1;
    counter_mock_screen_out_char = 0;
    tty_write_loop(&tty_table[0]);
    assert_int_equal(counter_mock_screen_out_char, 1);
}

static void test_tty_do_read_have_hooked_proc(void **state){
    Message msg;
    msg.mdata_tty_read.nr_tty = 1;
    mock_tty_table[1].have_hooked_proc = 1;
    assert_int_equal(tty_do_read(&msg), 0);
}

static void test_tty_do_read_dont_have_hooked_proc(void **state){
    Message msg;
    msg.mdata_tty_read.nr_tty = 1;
    mock_tty_table[1].have_hooked_proc = 0;
    will_return(mock_get_process_phy_mem, 0);
    assert_int_equal(tty_do_read(&msg), 1);
}

static void test_tty_do_write(void **state){
    char buf[10];
    buf[0] = 'a';
    Message msg;
    msg.mdata_tty_write.nr_tty = 0;
    msg.mdata_tty_write.buf = (uint32_t)buf;
    msg.mdata_tty_write.len = 1;
    mock_tty_table[1].have_hooked_proc = 1;
    will_return(mock_get_process_phy_mem, buf);
    tty_table[0].keyBuffer.buffer[0] = 'b';
    tty_table[0].keyBuffer.head = tty_table[0].keyBuffer.buffer;
    assert_int_equal(tty_do_write(&msg), 1);
    assert_int_equal((int)(tty_table[0].keyBuffer.buffer[0]), (int)'a');
}

int main(int argc, char* argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_switch_tty),
        cmocka_unit_test(test_tty_init),
        cmocka_unit_test(test_tty_read_loop_without_input),
        cmocka_unit_test(test_tty_read_loop_switch_tty_cmd),
        cmocka_unit_test(test_tty_read_loop_debug_cmd),
        cmocka_unit_test(test_tty_read_loop_enter),
        cmocka_unit_test(test_tty_read_loop_regular_key),
        cmocka_unit_test(test_tty_write_loop),
        cmocka_unit_test(test_tty_do_read_have_hooked_proc),
        cmocka_unit_test(test_tty_do_read_dont_have_hooked_proc),
        cmocka_unit_test(test_tty_do_write)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

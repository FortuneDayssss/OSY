#include "type.h"
#include "systemcall.h"
#include "print.h"
#include "keyboard.h"
#include "global.h"

void sys_test(){
    upRollScreen();
    printString("test!\n", -1);
}

uint64_t get_ticks(){
    return sys_ticks;
}

void print_keyboard_buffer(){
    keyboard_read();
}

//uint32_t test();
_system_call0(uint32_t,test)

//uint32_t times();
_system_call0(uint32_t,times)

//uint32_t getcharfromkeybuffer(uint8_t* buf, uint32_t buf_size);
_system_call2(uint32_t,getcharfromkeybuffer,uint8_t*,buf,uint32_t,size);

system_call_handler system_call_table[SYSTEM_CALL_NUM] = {
    (system_call_handler)sys_test,
    (system_call_handler)get_ticks,
    (system_call_handler)sys_keyboard_read,
    0
};
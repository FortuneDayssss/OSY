#include "type.h"
#include "systemcall.h"
#include "print.h"
#include "global.h"

void test(){
    upRollScreen();
    printString("test!\n", -1);
}

uint64_t get_ticks(){
    return sys_ticks;
}

system_call_handler system_call_table[SYSTEM_CALL_NUM] = {
    test,
    (system_call_handler)get_ticks,
    0
};
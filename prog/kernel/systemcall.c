#include "type.h"
#include "systemcall.h"
#include "print.h"

void test(){
    upRollScreen();
    printString("test!\n", -1);
}

system_call_handler system_call_table[SYSTEM_CALL_NUM] = {
    test,
    0
};
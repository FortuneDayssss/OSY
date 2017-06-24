#include "type.h"
#include "global.h"
#include "print.h"
#include "string.h"

//memory data
void* memdata_ptr;
uint32_t memdata[65];


void init_kernel(){
    //save and print memory data
    printInt32((uint32_t)memdata_ptr);
    upRollScreen();
    memcpy(&memdata, memdata_ptr, 260);
    printString("memory data:\n", -1);
    printString("table size:\n", -1);
    printInt32(memdata[0]);
    printString("\nbaseLow    baseHigh   lengthLow  lengthHigh type\n", -1);
    for(int i = 1; i < memdata[0] * 5 + 1; i++){
        printInt32(memdata[i]);
        printString("   ", -1);
        if((i) % 5 == 0)
            upRollScreen();
    }



}
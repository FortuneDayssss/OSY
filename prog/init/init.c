#include "type.h"
#include "global.h"
#include "print.h"
#include "protect.h"
#include "process.h"
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

    //move gdt
    memcpy(
        &gdt,
        (void*)(*((uint32_t*)(&gdt_ptr[2]))),
        *((uint16_t*)(&gdt_ptr[0])) + 1
    );
    uint16_t* gdt_limit_ptr = (uint16_t*)(&gdt_ptr[0]);
    uint32_t* gdt_base_ptr = (uint32_t*)(&gdt_ptr[2]);
    *gdt_limit_ptr = GDT_SIZE * sizeof(Descriptor) - 1;
    *gdt_base_ptr = (uint32_t)&gdt;

    //init idt
    uint16_t* idt_limit_ptr = (uint16_t*)(&idt_ptr[0]);
    uint32_t* idt_base_ptr = (uint32_t*)(&idt_ptr[2]);
    *idt_limit_ptr = IDT_SIZE * sizeof(Gate) - 1;
    *idt_base_ptr = (uint32_t)&idt;
    init_interrupt();
    init_tss_descriptor();

    //todo: delete
    // //init ldt descriptor in gdt
    // for(int i = 0; i < MAX_PROCESS_NUM; i++){
    //     init_descriptor(
    //         &gdt[(SELECTOR_LDT_FIRST + 8 * i) >> 3],
    //         (uint32_t)vir2phyaddr(seg2phyaddr(SELECTOR_MEMC_0), &(pcb_table[i].ldt)),
    //         LDT_SIZE * sizeof(Descriptor) - 1,
    //         DA_LDT
    //     );
    // }

}
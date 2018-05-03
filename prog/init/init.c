#include "type.h"
#include "global.h"
#include "print.h"
#include "protect.h"
#include "process.h"
#include "string.h"
#include "keyboard.h"
#include "mm.h"

// memory data (get from bios) fromat
#define MEM_DATA_FROM_BIOS_TYPE_AVAILABLE 1
typedef struct{
    uint32_t base_low;
    uint32_t base_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
}Mem_Data_From_Bios_Entry;

typedef struct{
    uint32_t table_size;
    Mem_Data_From_Bios_Entry table[0];
}Mem_Data_From_Bios;

// memory data
uint32_t* memdata_ptr;

void init_kernel(){
    // print memory data
    printInt32((uint32_t)memdata_ptr);
    upRollScreen();
    Mem_Data_From_Bios* memdata = (Mem_Data_From_Bios*)memdata_ptr;
    printString("memory data:\n", -1);
    printString("table size:\n", -1);
    printInt32(memdata->table_size);
    printString("\nbaseHigh   baseLow    lengthHigh lengthLow  type       available\n", -1);
    for(int i = 0; i < memdata->table_size; i++){
        printInt32(memdata->table[i].base_high);printString("   ", -1);
        printInt32(memdata->table[i].base_low);printString("   ", -1);
        printInt32(memdata->table[i].length_high);printString("   ", -1);
        printInt32(memdata->table[i].length_low);printString("   ", -1);
        printInt32(memdata->table[i].type);printString("   ", -1);
        if(memdata->table[i].type == MEM_DATA_FROM_BIOS_TYPE_AVAILABLE){
            printString("true", -1);
        }
        printString("\n", -1);
    }

    // init page table
    for(uint32_t i = 0; i < memdata->table_size; i++){
        if(memdata->table[i].type == MEM_DATA_FROM_BIOS_TYPE_AVAILABLE){
            uint32_t base = memdata->table[i].base_low;
            uint32_t len = memdata->table[i].length_low;

            for(int pde_nr = 0; pde_nr < 1024; pde_nr++){ // scan pde table
                uint32_t pde_base = pde_nr << 22;
                uint32_t pde_end = ((pde_nr + 1) << 22) - 1;
                if(pde_end >= base && pde_base <= (base + len)){ // have any memory in the range
                    uint32_t* pte_table = (uint32_t*)((kernel_page_table->pde[pde_nr]) & PDE_BASE_ADDR_MASK);
                    for(int pte_nr = 0; pte_nr < 1024; pte_nr++){ // scan pte table
                        uint32_t pte_base = pde_base + (pte_nr << 12);
                        uint32_t pte_end = pde_base + ((pte_nr + 1) << 12);
                        if(pte_base >= base && pte_end <= (base + len)){ // if all mem in pte is in the range, set pte and pde available
                            pte_table[pte_nr] &= (~PTE_OSY_TYPE_MASK);
                            pte_table[pte_nr] |= (PTE_OSY_TYPE_AVAILABLE);
                            
                            (kernel_page_table->pde[pde_nr]) &= (~PDE_OSY_TYPE_MASK);
                            (kernel_page_table->pde[pde_nr]) |= (PDE_OSY_TYPE_AVAILABLE);
                        } // end if
                    }// end scan pte loop
                }// end if
            }// end scan pde loop

        }
    }
    printString("page table init ok\n", -1);

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
    init_clock();
    init_keyboard();
    init_hd();
    init_tss_descriptor();
    init_ldt();

    //gmem state (for debug)
    uint16_t temp;
    uint8_t* p = (uint8_t*)(&temp);
    out_byte(CRTC_ADDR_REG, START_ADDR_H);
    *p = in_byte(CRTC_DATA_REG);
    out_byte(CRTC_ADDR_REG, START_ADDR_L);
    p++;
    *p = in_byte(CRTC_DATA_REG);
    printString("gmem start: ", -1);
    printInt16(temp);
    upRollScreen();
}
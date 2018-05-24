#include "tty.h"
#include "systemcall.h"
#include "type.h"
#include "print.h"
#include "global.h"
#include "keyboard.h"
#include "string.h"
#include "debug.h"

//screen
void screen_out_char(int nr_tty, char ch){
    if(tty_table[nr_tty].console.cursorAddr - tty_table[nr_tty].console.graphMemoryBase >= (80*25-1)*2){
        screen_roll_up(nr_tty);
        tty_table[nr_tty].console.cursorAddr -= (tty_table[nr_tty].console.cursorAddr - tty_table[nr_tty].console.graphMemoryBase) % 160;
    }
    if(ch == '\n'){
        screen_roll_up(nr_tty);
        tty_table[nr_tty].console.cursorAddr -= (tty_table[nr_tty].console.cursorAddr - tty_table[nr_tty].console.graphMemoryBase) % 160;
    }
    else{
        uint16_t out_ch = 0x0F00 | (0x00FF & ch);
        *((uint16_t *)(tty_table[nr_tty].console.cursorAddr + V_MEM_BASE)) = out_ch;
        tty_table[nr_tty].console.cursorAddr += 2;
    }
}

void screen_roll_up(int nr_tty){
    uint16_t* screen_ptr = (uint16_t*)(tty_table[nr_tty].console.graphMemoryBase + V_MEM_BASE);
    for(int i = 0; i < 80*24; i++){//vga text mode h=25, w=80
        *screen_ptr = *(screen_ptr + 80);
        screen_ptr++;
    }
    screen_ptr = (uint16_t*)(tty_table[nr_tty].console.graphMemoryBase + V_MEM_BASE);
    screen_ptr += 80*24;    
    for(int i = 0; i < 80 ; i++){
        *screen_ptr = 0;
        screen_ptr++;
    }
    tty_table[nr_tty].console.cursorAddr = tty_table[nr_tty].console.graphMemoryBase + 80*24*2;
}

void screen_set_base_addr(uint32_t addr){
    __asm__("cli\n\t" ::);
    addr = addr >> 1;
    out_byte(CRTC_ADDR_REG, START_ADDR_H);
    out_byte(CRTC_DATA_REG, (addr >> 8) & 0xff);
    out_byte(CRTC_ADDR_REG, START_ADDR_L);
    out_byte(CRTC_DATA_REG, addr & 0xff);
    __asm__("sti\n\t" ::);
}

void screen_set_cursor(uint32_t pos){
    __asm__("cli\n\t" ::);
    out_byte(CRTC_ADDR_REG, CURSOR_H);
    out_byte(CRTC_DATA_REG, (pos >> 8) & 0xff);
    out_byte(CRTC_ADDR_REG, CURSOR_L);
    out_byte(CRTC_DATA_REG, pos & 0xff);
    __asm__("sti\n\t" ::);
}

void screen_flush(TTY_Console *pc){
    screen_set_base_addr(pc->graphMemoryBase);
    screen_set_cursor(pc->cursorAddr);
}
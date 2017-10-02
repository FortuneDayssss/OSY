#include "tty.h"
#include "systemcall.h"
#include "type.h"
#include "print.h"
#include "global.h"
#include "keyboard.h"

//local function
void screen_out_char(int nr_tty, char ch);
void screen_set_cursor(uint32_t pos);
void screen_set_base_addr(uint32_t addr);
void screen_flush(TTY_Console *pc);

//tty
uint32_t sys_tty_write(uint8_t *buf, uint32_t size){
    for (int i = 0; (size == -1 || i < size) && buf[i] != '\0' && current_tty->keyBuffer.count < TTY_BUFFER_SIZE; i++){
        *(current_tty->keyBuffer.head) = buf[i];
        current_tty->keyBuffer.head++;
        if (current_tty->keyBuffer.head == current_tty->keyBuffer.buffer + TTY_BUFFER_SIZE)
            current_tty->keyBuffer.head = current_tty->keyBuffer.buffer;
        current_tty->keyBuffer.count++;
    }
}

void switch_tty(int n){
    if (n >= NR_TTYS || n < 0)
        return;
    current_tty = &tty_table[n];
    screen_flush(&(current_tty->console));
}

void tty_init(TTY *pt){
    tty_buffer_init(&(pt->keyBuffer));
    tty_console_init(&(pt->console), pt - tty_table);
}

//local parameters for keyboard
uint32_t code_with_E0 = 0;

void tty_read(TTY *pt){
    uint32_t key;
    uint32_t shift;
    uint32_t ctrl;
    uint32_t alt;
    uint32_t make;
    uint32_t have_input;
    do{
        have_input = keyboard_process(pt, &key, &shift, &ctrl, &alt, &make);
        if(have_input && (key != 0) && make == 1 && key >= '1' && key <= '9' && ctrl){
            switch_tty(key - '1');
        }
        else if(have_input && key != 0 && (!(key & FLAG_EXT)) && make == 1){
            *(pt->keyBuffer.head) = key;
            pt->keyBuffer.head++;
            if (pt->keyBuffer.head == pt->keyBuffer.buffer + TTY_BUFFER_SIZE)
                pt->keyBuffer.head = pt->keyBuffer.buffer;
            pt->keyBuffer.count++;
        }
    }while(have_input);
}

void tty_write(TTY *pt){
    if (pt->keyBuffer.count > 0){
        screen_out_char(pt - tty_table, *(pt->keyBuffer.tail));
        pt->keyBuffer.tail++;
        if (pt->keyBuffer.tail == pt->keyBuffer.buffer + TTY_BUFFER_SIZE)
            pt->keyBuffer.tail = pt->keyBuffer.buffer;
        pt->keyBuffer.count--;
    }
}

void tty_main(){

    //init tty
    for (int i = 0; i < NR_TTYS; i++)
        tty_init(&tty_table[i]);
    current_tty = tty_table;

    //tty loop
    while (1){
        tty_read(current_tty);
        tty_write(current_tty);
    }
}

//tty buffer
void tty_buffer_init(TTY_KeyBuffer *pb){
    pb->count = 0;
    pb->head = pb->buffer;
    pb->tail = pb->buffer;
}

//console
void tty_console_init(TTY_Console *pc, int nr_console){
    pc->graphMemoryBase = CONSOLE_GMEM_SIZE * nr_console;
    pc->dispAddr = pc->graphMemoryBase;
    pc->cursorAddr = pc->graphMemoryBase;
}

//screen
void screen_out_char(int nr_tty, char ch){
    uint16_t out_ch = 0x0F00 | (0x00FF & ch);
    *((uint16_t *)(current_tty->console.cursorAddr + V_MEM_BASE)) = out_ch;
    tty_table[nr_tty].console.cursorAddr += 2;
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
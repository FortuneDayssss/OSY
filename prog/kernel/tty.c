#include "tty.h"
#include "systemcall.h"
#include "type.h"
#include "print.h"
#include "global.h"


//local function
void screen_set_cursor(uint32_t pos);
void screen_set_base_addr(uint32_t addr);
void screen_flush(TTY_Console* pc);

//tty
uint32_t sys_tty_write(uint8_t* buf, uint32_t size){
    for(int i = 0; (size == -1 || i < size) 
        && buf[i] != '\0' && current_tty->keyBuffer.count < TTY_BUFFER_SIZE; i++){
        *(current_tty->keyBuffer.head) = buf[i];
        current_tty->keyBuffer.head++;
        if(current_tty->keyBuffer.head == current_tty->keyBuffer.buffer + TTY_BUFFER_SIZE)
            current_tty->keyBuffer.head = current_tty->keyBuffer.buffer;
        current_tty->keyBuffer.count++;
    }
}

void switch_tty(int n){
    if(n >= NR_TTYS || n < 0)
        return;
    current_tty = &tty_table[n];
}

void tty_init(TTY* pt){
    tty_buffer_init(&(pt->keyBuffer));
    tty_console_init(&(pt->console), pt - tty_table);
}

void tty_read(TTY* pt){
    uint8_t key_buf[32];
    int read_size = getcharfromkeybuffer(key_buf, 32);
    if(read_size > 0){
        for(int i = 0; i < read_size && pt->keyBuffer.count < TTY_BUFFER_SIZE; i++){
            *(pt->keyBuffer.head) = key_buf[i];
            pt->keyBuffer.head++;
            if(pt->keyBuffer.head == pt->keyBuffer.buffer + TTY_BUFFER_SIZE)
                pt->keyBuffer.head = pt->keyBuffer.buffer;
            pt->keyBuffer.count++;
        }
    }
}

void tty_write(TTY* pt){
    if(pt->keyBuffer.count > 0){
        printChar(pt->keyBuffer.tail);
        pt->keyBuffer.tail++;
        if(pt->keyBuffer.tail == pt->keyBuffer.buffer + TTY_BUFFER_SIZE)
            pt->keyBuffer.tail = pt->keyBuffer.buffer;
        pt->keyBuffer.count--;
    }
}

void tty_main(){

    //init tty
    for(int i = 0; i < NR_TTYS; i++)
        tty_init(&tty_table[i]);
    current_tty = tty_table;
    
    //tty loop
    while(1){
        tty_read(current_tty);
        tty_write(current_tty);
    }
}

//tty buffer
void tty_buffer_init(TTY_KeyBuffer* pb){
    pb->count = 0;
    pb->head = pb->buffer;
    pb->tail = pb->buffer;
}

//console
void tty_console_init(TTY_Console* pc, int nr_console){
    pc->graphMemoryBase = V_MEM_BASE + CONSOLE_GMEM_SIZE * nr_console;
    pc->dispAddr = pc->graphMemoryBase;
    pc->cursorAddr = pc->graphMemoryBase;
}

//screen
void screen_switch(int nr_tty){
    if(nr_tty < 0 || nr_tty >= NR_CONSOLES)
        return;

    current_tty = tty_table + nr_tty;
    
    screen_flush(&(current_tty->console));
}

void screen_set_base_addr(uint32_t addr){
    __asm__("cli\n\t"::);
    out_byte(CRTC_ADDR_REG, START_ADDR_H);
    out_byte(CRTC_DATA_REG, (addr >> 8) & 0xff);
    out_byte(CRTC_ADDR_REG, START_ADDR_L);
    out_byte(CRTC_DATA_REG, addr & 0xff);
    __asm__("sti\n\t"::);
}

void screen_set_cursor(uint32_t pos){
    __asm__("cli\n\t"::);
    out_byte(CRTC_ADDR_REG, CURSOR_H);
    out_byte(CRTC_DATA_REG, (pos >> 8) & 0xff);
    out_byte(CRTC_ADDR_REG, CURSOR_L);
    out_byte(CRTC_DATA_REG, pos & 0xff);
    __asm__("sti\n\t"::);
}

void screen_flush(TTY_Console* pc){
    screen_set_base_addr(pc->graphMemoryBase);
    screen_set_cursor(pc->cursorAddr);
}
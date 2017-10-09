#include "tty.h"
#include "systemcall.h"
#include "type.h"
#include "print.h"
#include "global.h"
#include "keyboard.h"

//local function
void screen_out_char(int nr_tty, char ch);
void screen_roll_up(int nr_tty);
void screen_set_cursor(uint32_t pos);
void screen_set_base_addr(uint32_t addr);
void screen_flush(TTY_Console *pc);

//tty
uint32_t sys_tty_write(uint8_t *buf, uint32_t size){
    for (int i = 0; (size == -1 || i < size) && buf[i] != '\0' && current_process->tty->keyBuffer.count < TTY_BUFFER_SIZE; i++){
        *(current_process->tty->keyBuffer.head) = buf[i];
        current_process->tty->keyBuffer.head++;
        if (current_process->tty->keyBuffer.head == current_process->tty->keyBuffer.buffer + TTY_BUFFER_SIZE)
        current_process->tty->keyBuffer.head = current_process->tty->keyBuffer.buffer;
        current_process->tty->keyBuffer.count++;
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

void tty_read_loop(TTY *pt){
    uint32_t key;
    uint32_t shift;
    uint32_t ctrl;
    uint32_t alt;
    uint32_t make;
    uint32_t have_input;
    do{
        have_input = keyboard_process(pt, &key, &shift, &ctrl, &alt, &make);
        if(have_input && (key != 0) && make == 1 && key >= '1' && key <= '9' && ctrl){//ctrl+1~9, switch tty
            switch_tty(key - '1');
        }
        else if(have_input && key ==ENTER && make == 1){                    //enter, roll up screen
            screen_roll_up(pt - tty_table);
        }
        else if(have_input && key != 0 && (!(key & FLAG_EXT)) && make == 1){//screen output
            *(pt->keyBuffer.head) = key;
            pt->keyBuffer.head++;
            if (pt->keyBuffer.head == pt->keyBuffer.buffer + TTY_BUFFER_SIZE)
                pt->keyBuffer.head = pt->keyBuffer.buffer;
            pt->keyBuffer.count++;
        }
    }while(have_input);
}

void tty_write_loop(TTY *pt){
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
        tty_read_loop(current_tty);
        tty_write_loop(current_tty);
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
    pc->cursorAddr = pc->graphMemoryBase + 80*24*2;
}

//screen
void screen_out_char(int nr_tty, char ch){
    if(tty_table[nr_tty].console.cursorAddr - tty_table[nr_tty].console.graphMemoryBase >= (80*25-1)*2){
        screen_roll_up(nr_tty);
        tty_table[nr_tty].console.cursorAddr -= tty_table[nr_tty].console.cursorAddr % 160;
    }
    if(ch == '\n'){
        screen_roll_up(nr_tty);
        tty_table[nr_tty].console.cursorAddr -= tty_table[nr_tty].console.cursorAddr % 160;
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
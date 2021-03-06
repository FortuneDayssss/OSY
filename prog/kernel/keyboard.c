#include "print.h"
#include "protect.h"
#include "global.h"
#include "keyboard.h"
#include "type.h"
#include "tty.h"

//keyboard map(from Orange'S)
uint32_t keymap[128 * 3] = {
/* scan-code			!Shift		Shift		E0 XX	*/
/* ==================================================================== */
/* 0x00 - none		*/	0,		    0,		    0,
/* 0x01 - ESC		*/	ESC,		ESC,		0,
/* 0x02 - '1'		*/	'1',		'!',		0,
/* 0x03 - '2'		*/	'2',		'@',		0,
/* 0x04 - '3'		*/	'3',		'#',		0,
/* 0x05 - '4'		*/	'4',		'$',		0,
/* 0x06 - '5'		*/	'5',		'%',		0,
/* 0x07 - '6'		*/	'6',		'^',		0,
/* 0x08 - '7'		*/	'7',		'&',		0,
/* 0x09 - '8'		*/	'8',		'*',		0,
/* 0x0A - '9'		*/	'9',		'(',		0,
/* 0x0B - '0'		*/	'0',		')',		0,
/* 0x0C - '-'		*/	'-',		'_',		0,
/* 0x0D - '='		*/	'=',		'+',		0,
/* 0x0E - BS		*/	BACKSPACE,	BACKSPACE,	0,
/* 0x0F - TAB		*/	TAB,		TAB,		0,
/* 0x10 - 'q'		*/	'q',		'Q',		0,
/* 0x11 - 'w'		*/	'w',		'W',		0,
/* 0x12 - 'e'		*/	'e',		'E',		0,
/* 0x13 - 'r'		*/	'r',		'R',		0,
/* 0x14 - 't'		*/	't',		'T',		0,
/* 0x15 - 'y'		*/	'y',		'Y',		0,
/* 0x16 - 'u'		*/	'u',		'U',		0,
/* 0x17 - 'i'		*/	'i',		'I',		0,
/* 0x18 - 'o'		*/	'o',		'O',		0,
/* 0x19 - 'p'		*/	'p',		'P',		0,
/* 0x1A - '['		*/	'[',		'{',		0,
/* 0x1B - ']'		*/	']',		'}',		0,
/* 0x1C - CR/LF		*/	ENTER,		ENTER,		PAD_ENTER,
/* 0x1D - l. Ctrl	*/	CTRL_L,		CTRL_L,		CTRL_R,
/* 0x1E - 'a'		*/	'a',		'A',		0,
/* 0x1F - 's'		*/	's',		'S',		0,
/* 0x20 - 'd'		*/	'd',		'D',		0,
/* 0x21 - 'f'		*/	'f',		'F',		0,
/* 0x22 - 'g'		*/	'g',		'G',		0,
/* 0x23 - 'h'		*/	'h',		'H',		0,
/* 0x24 - 'j'		*/	'j',		'J',		0,
/* 0x25 - 'k'		*/	'k',		'K',		0,
/* 0x26 - 'l'		*/	'l',		'L',		0,
/* 0x27 - ';'		*/	';',		':',		0,
/* 0x28 - '\''		*/	'\'',		'"',		0,
/* 0x29 - '`'		*/	'`',		'~',		0,
/* 0x2A - l. SHIFT	*/	SHIFT_L,	SHIFT_L,	0,
/* 0x2B - '\'		*/	'\\',		'|',		0,
/* 0x2C - 'z'		*/	'z',		'Z',		0,
/* 0x2D - 'x'		*/	'x',		'X',		0,
/* 0x2E - 'c'		*/	'c',		'C',		0,
/* 0x2F - 'v'		*/	'v',		'V',		0,
/* 0x30 - 'b'		*/	'b',		'B',		0,
/* 0x31 - 'n'		*/	'n',		'N',		0,
/* 0x32 - 'm'		*/	'm',		'M',		0,
/* 0x33 - ','		*/	',',		'<',		0,
/* 0x34 - '.'		*/	'.',		'>',		0,
/* 0x35 - '/'		*/	'/',		'?',		PAD_SLASH,
/* 0x36 - r. SHIFT	*/	SHIFT_R,	SHIFT_R,	0,
/* 0x37 - '*'		*/	'*',		'*',    	0,
/* 0x38 - ALT		*/	ALT_L,		ALT_L,  	ALT_R,
/* 0x39 - ' '		*/	' ',		' ',		0,
/* 0x3A - CapsLock	*/	CAPS_LOCK,	CAPS_LOCK,	0,
/* 0x3B - F1		*/	F1,		F1,		0,
/* 0x3C - F2		*/	F2,		F2,		0,
/* 0x3D - F3		*/	F3,		F3,		0,
/* 0x3E - F4		*/	F4,		F4,		0,
/* 0x3F - F5		*/	F5,		F5,		0,
/* 0x40 - F6		*/	F6,		F6,		0,
/* 0x41 - F7		*/	F7,		F7,		0,
/* 0x42 - F8		*/	F8,		F8,		0,
/* 0x43 - F9		*/	F9,		F9,		0,
/* 0x44 - F10		*/	F10,		F10,		0,
/* 0x45 - NumLock	*/	NUM_LOCK,	NUM_LOCK,	0,
/* 0x46 - ScrLock	*/	SCROLL_LOCK,	SCROLL_LOCK,	0,
/* 0x47 - Home		*/	PAD_HOME,	'7',		HOME,
/* 0x48 - CurUp		*/	PAD_UP,		'8',		UP,
/* 0x49 - PgUp		*/	PAD_PAGEUP,	'9',		PAGEUP,
/* 0x4A - '-'		*/	PAD_MINUS,	'-',		0,
/* 0x4B - Left		*/	PAD_LEFT,	'4',		LEFT,
/* 0x4C - MID		*/	PAD_MID,	'5',		0,
/* 0x4D - Right		*/	PAD_RIGHT,	'6',		RIGHT,
/* 0x4E - '+'		*/	PAD_PLUS,	'+',		0,
/* 0x4F - End		*/	PAD_END,	'1',		END,
/* 0x50 - Down		*/	PAD_DOWN,	'2',		DOWN,
/* 0x51 - PgDown	*/	PAD_PAGEDOWN,	'3',		PAGEDOWN,
/* 0x52 - Insert	*/	PAD_INS,	'0',		INSERT,
/* 0x53 - Delete	*/	PAD_DOT,	'.',		DELETE,
/* 0x54 - Enter		*/	0,		0,		0,
/* 0x55 - ???		*/	0,		0,		0,
/* 0x56 - ???		*/	0,		0,		0,
/* 0x57 - F11		*/	F11,		F11,		0,	
/* 0x58 - F12		*/	F12,		F12,		0,	
/* 0x59 - ???		*/	0,		0,		0,	
/* 0x5A - ???		*/	0,		0,		0,	
/* 0x5B - ???		*/	0,		0,		GUI_L,	
/* 0x5C - ???		*/	0,		0,		GUI_R,	
/* 0x5D - ???		*/	0,		0,		APPS,	
/* 0x5E - ???		*/	0,		0,		0,	
/* 0x5F - ???		*/	0,		0,		0,
/* 0x60 - ???		*/	0,		0,		0,
/* 0x61 - ???		*/	0,		0,		0,	
/* 0x62 - ???		*/	0,		0,		0,	
/* 0x63 - ???		*/	0,		0,		0,	
/* 0x64 - ???		*/	0,		0,		0,	
/* 0x65 - ???		*/	0,		0,		0,	
/* 0x66 - ???		*/	0,		0,		0,	
/* 0x67 - ???		*/	0,		0,		0,	
/* 0x68 - ???		*/	0,		0,		0,	
/* 0x69 - ???		*/	0,		0,		0,	
/* 0x6A - ???		*/	0,		0,		0,	
/* 0x6B - ???		*/	0,		0,		0,	
/* 0x6C - ???		*/	0,		0,		0,	
/* 0x6D - ???		*/	0,		0,		0,	
/* 0x6E - ???		*/	0,		0,		0,	
/* 0x6F - ???		*/	0,		0,		0,	
/* 0x70 - ???		*/	0,		0,		0,	
/* 0x71 - ???		*/	0,		0,		0,	
/* 0x72 - ???		*/	0,		0,		0,	
/* 0x73 - ???		*/	0,		0,		0,	
/* 0x74 - ???		*/	0,		0,		0,	
/* 0x75 - ???		*/	0,		0,		0,	
/* 0x76 - ???		*/	0,		0,		0,	
/* 0x77 - ???		*/	0,		0,		0,	
/* 0x78 - ???		*/	0,		0,		0,	
/* 0x78 - ???		*/	0,		0,		0,	
/* 0x7A - ???		*/	0,		0,		0,	
/* 0x7B - ???		*/	0,		0,		0,	
/* 0x7C - ???		*/	0,		0,		0,	
/* 0x7D - ???		*/	0,		0,		0,	
/* 0x7E - ???		*/	0,		0,		0,
/* 0x7F - ???		*/	0,		0,		0
};

//keyboard variables
KeyboardBuffer  keyboard_buffer;
int             shift_left;
int             shift_right;
int             alt_left;
int             alt_right;
int             ctrl_left;
int             ctrl_right;
int             caps_lock;
int             num_lock;
int             scroll_lock;
int	            code_with_E0;

//local function declaration
void set_leds();
uint8_t keyboard_buffer_get();
uint8_t keyboard_buffer_getCode(int seek);
void keyboard_buffer_pop(int pop_len);

//keyboard.c
void keyboard_handler(){
    uint8_t scan_code = in_byte(KB_DATA);
    
    if(keyboard_buffer.count < KEYBOARD_BUFFER_MAX_SIZE){
        *(keyboard_buffer.head) = scan_code;
        keyboard_buffer.head++;
        if(keyboard_buffer.head == keyboard_buffer.buffer + KEYBOARD_BUFFER_MAX_SIZE)
            keyboard_buffer.head = keyboard_buffer.buffer;
        keyboard_buffer.count++;
    }
}

int keyboard_process(TTY* tty, uint32_t* key_ret, uint32_t* shift_ret, uint32_t* ctrl_ret, uint32_t* alt_ret, uint32_t* make_ret){
    if(keyboard_buffer.count <= 0)
        return 0;
    uint8_t scan_code;
    uint32_t make = 0;
    uint32_t key = 0;
    uint32_t* keyrow;
    uint32_t seek = 0;
    if(keyboard_buffer.count > 0){
        code_with_E0 = 0;
        scan_code = keyboard_buffer_getCode(seek);

        //scan code size > 3
        if(scan_code == 0xE1){ //pause break
            if(keyboard_buffer.count >= 6 &&
                keyboard_buffer_getCode(seek + 1) == 0x1D &&
                keyboard_buffer_getCode(seek + 2) == 0x45 &&
                keyboard_buffer_getCode(seek + 3) == 0xE1 &&
                keyboard_buffer_getCode(seek + 4) == 0x9D &&
                keyboard_buffer_getCode(seek + 5) == 0xC5){
                key = PAUSEBREAK;
                keyboard_buffer_pop(6);
            }
        }else if(scan_code == 0xE0){//screenprint
            if(keyboard_buffer.count >= 4 &&
                keyboard_buffer_getCode(seek + 1) == 0x2A &&
                keyboard_buffer_getCode(seek + 2) == 0xE0 &&
                keyboard_buffer_getCode(seek + 3) == 0x37){
                key = PRINTSCREEN;
                make = 1;
                keyboard_buffer_pop(4);
            }else if(keyboard_buffer.count >= 4 &&
                keyboard_buffer_getCode(seek + 1) == 0xB7 &&
                keyboard_buffer_getCode(seek + 2) == 0xE0 &&
                keyboard_buffer_getCode(seek + 3) == 0xAA){
                key = PRINTSCREEN;
                make = 0;
                keyboard_buffer_pop(4);
            }
            if(key == 0){
                code_with_E0 = 1;
            }
        }

        //scan code size <= 2
        if((key != PAUSEBREAK) && (key != PRINTSCREEN)){
            make = (scan_code & FLAG_BREAK ? 0 : 1);
            uint32_t* keyrow = &keymap[(scan_code & 0x7F) * MAP_COLS];
            uint32_t column = 0;

            int caps = shift_left || shift_right;
            if(caps_lock && (keyrow[0] >= 'a') && (keyrow[0] <= 'z')){
                caps = !caps;
            }
            if (caps) {
				column = 1;
			}
			if (code_with_E0) {
				column = 2;
            }
            key = keyrow[column];

            switch(key){
                case SHIFT_L:
                    shift_left = make;
                    break;
                case SHIFT_R:
                    shift_right = make;
                    break;
                case CTRL_L:
                    ctrl_left = make;
                    break;
                case CTRL_R:
                    ctrl_right = make;
                    break;
                case ALT_L:
                    alt_left = make;
                    break;
                case ALT_R:
                    alt_right = make;
                    break;
                case CAPS_LOCK:
                    if(make){
                        caps_lock = !caps_lock;
                        set_leds();
                    }
                    break;
                case NUM_LOCK:
                    if(make){
                        num_lock = !num_lock;
                        set_leds();
                    }
                    break;
                case SCROLL_LOCK:
                    if(make){
                        scroll_lock = !scroll_lock;
                        set_leds();
                    }
                    break;
                default:
                    break;
            }

            if(make){
                int pad = 0;
                if((key >= PAD_SLASH) && (key <= PAD_9)){
                    pad = 1;
                    switch(key){
                        case PAD_SLASH:
                            key = '/';
                            break;
                        case PAD_STAR:
                            key = '*';
                            break;
                        case PAD_MINUS:
                            key = '-';
                            break;
                        case PAD_PLUS:
                            key = '+';
                            break;
                        case PAD_ENTER:
                            key = ENTER;
                            break;
                        default:
                            if(num_lock &&
                                (key >= PAD_0) &&
                                (key <= PAD_9)){
                                    key = key - PAD_0 + '0';
                            }
                            else if(num_lock && (key == PAD_DOT)){
                                key = '.';
                            }
                            else{
                                switch(key){
                                    case PAD_HOME:
                                        key = HOME;
                                        break;
                                    case PAD_END:
                                        key = END;
                                        break;
                                    case PAD_PAGEUP:
                                        key = PAD_PAGEUP;
                                        break;
                                    case PAD_PAGEDOWN:
                                        key = PAGEDOWN;
                                        break;
                                    case PAD_INS:
                                        key = INSERT;
                                        break;
                                    case PAD_UP:
                                        key = UP;
                                        break;
                                    case PAD_DOWN:
                                        key = DOWN;
                                        break;
                                    case PAD_LEFT:
                                        key = LEFT;
                                        break;
                                    case PAD_RIGHT:
                                        key = RIGHT;
                                        break;
                                    case PAD_DOT:
                                        key = DELETE;
                                        break;
                                    default:
                                        break;
                                }
                            }
                            break;
                    }
                }
            }
            if(scan_code == 0xE0){
                keyboard_buffer_pop(2);
            }
            else{
                keyboard_buffer_pop(1);
            }
        }
    }
    
    *key_ret = key;
    *shift_ret = shift_left | shift_right;
    *ctrl_ret = ctrl_left | ctrl_right;
    *alt_ret = alt_left | alt_right;
    *make_ret = make;
    return 1;
}

void init_keyboard(){
    keyboard_buffer.count = 0;
    keyboard_buffer.head = &keyboard_buffer.buffer[0];
    keyboard_buffer.tail = &keyboard_buffer.buffer[0];
    shift_left = 0;
    shift_right = 0;
    alt_left = 0;
    alt_right = 0;
    caps_lock = 0;
    num_lock = 1;
    scroll_lock = 0;
    set_leds();

    // init keyboard irq handler
    set_irq_handler(KEYBOARD_IRQ, keyboard_handler);
}

uint8_t keyboard_buffer_get(){
    uint8_t scan_code;
    // while(keyboard_buffer.count <= 0){}

    __asm__("cli\n\t");

    scan_code = *(keyboard_buffer.tail);
    keyboard_buffer.tail++;
    if(keyboard_buffer.tail >= keyboard_buffer.buffer + KEYBOARD_BUFFER_MAX_SIZE)
        keyboard_buffer.tail = keyboard_buffer.buffer;
    keyboard_buffer.count--;

    __asm__("sti\n\t");

    return scan_code;
}

uint8_t keyboard_buffer_getCode(int seek){
    if(seek > keyboard_buffer.count)
        return 0;
    uint8_t scan_code;
        
    __asm__("cli\n\t");
    scan_code = keyboard_buffer.buffer[((uint32_t)(keyboard_buffer.tail - keyboard_buffer.buffer) + seek) % KEYBOARD_BUFFER_MAX_SIZE];
    __asm__("sti\n\t");
        
    return scan_code;
}

void keyboard_buffer_pop(int pop_len){
    if(pop_len > keyboard_buffer.count)
        return;
    __asm__("cli\n\t");
    for(int i = 0; i < pop_len; i++){
        keyboard_buffer.tail++;
        if(keyboard_buffer.tail >= keyboard_buffer.buffer + KEYBOARD_BUFFER_MAX_SIZE)
            keyboard_buffer.tail = keyboard_buffer.buffer;
    }
    keyboard_buffer.count -= pop_len;
    __asm__("sti\n\t");
}

void set_leds(){
    uint8_t temp;
    uint8_t leds = (caps_lock << 2)|(num_lock << 1)|scroll_lock;

    do {                            //等待8042缓冲区空
        temp = in_byte(KB_CMD);
    }while(temp & 0x02);
    out_byte(KB_DATA, LED_CODE);    //输出 led code
    do{                             //等待ack
        temp = in_byte(KB_DATA);
    }while(temp != KB_ACK);


    do {                            //等待8042缓冲区空
        temp = in_byte(KB_CMD);
    }while(temp & 0x02);
    out_byte(KB_DATA, leds);        //输出 led data
    do{                             //等待ack
        temp = in_byte(KB_DATA);
    }while(temp != KB_ACK);
}
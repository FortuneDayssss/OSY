BOOTSEC_SEG             equ     0000h
SETUP_SEG               equ     9000h
KERNEL_SEG              equ     5000h
STACK_SEG               equ     9000h
STACK_TOP               equ     0ff00h

KERNEL_ADDR             equ     0000h           ;00050000 => kernel elf
SETUP_ADDR              equ     0000h           ;00090000 => setup

SEC_PER_SEG             equ     128
FLOOPPY_DRIVE           equ     0

SEC_PER_CYLINDER_PLUS_1 equ     19
CYLINDER_PER_HEAD       equ     80
HEAD_PER_FLOPPY         equ     2

SETUP_START_HEAD        equ     0
SETUP_START_CYLINDER    equ     0
SETUP_START_SEC         equ     2
SETUP_SEC_NUM           equ     3

KERNEL_START_HEAD       equ     0
KERNEL_START_CYLINDER   equ     0
KERNEL_START_SEC        equ     5
KERNEL_SEC_NUM          equ     30




org     00007c00h

[SECTION .bootsec]
[BITS 16]
    mov     ax,     STACK_SEG
    mov     ss,     ax
    mov     sp,     STACK_TOP
    mov     ax,     BOOTSEC_SEG
    mov     ds,     ax
    mov     ax,     8000h
    mov     es,     ax
    mov     bx,     7c00h
    mov     cx,     511
COPY_BOOTSEC_LOOP:
    mov     eax,    [ds : bx]
    mov     [es : bx],  eax
    add     bx,     1
    loop    COPY_BOOTSEC_LOOP
    jmp     8000h:COPY_BOOTSEC_OK

COPY_BOOTSEC_OK:   

;-----------load setup sectors------------------------------------------------------------
    mov     al,     SETUP_START_HEAD
    mov     [HEAD], al
    mov     al,     SETUP_START_CYLINDER
    mov     [CYLINDER], al
    mov     al,     SETUP_START_SEC
    mov     [SECTOR],   al
    xor     eax,    eax
    mov     ax,     SETUP_SEG
    shl     eax,    4
    add     eax,    SETUP_ADDR
    mov     [DEST_ADDR],    eax
    mov     eax,    0
    mov     [LOADED_NUM],   eax

LOAD_SETUP_LOOP:
    mov     dl,     FLOOPPY_DRIVE       ;driver
    mov     dh,     [HEAD]              ;head
    mov     ch,     [CYLINDER]          ;cylinder
    mov     cl,     [SECTOR]            ;sector
    mov     eax,    [DEST_ADDR]
    and     eax,    000f0000h
    shr     eax,    4
    mov     es,     ax                  ;segment
    mov     eax,    [DEST_ADDR]
    mov     bx,     ax                  ;address
    mov     ah,     02h                 ;read sectors from drive
    mov     al,     01h                 ;sectors to read count
    int     13h

    inc byte    [SECTOR]
    mov     al,     [SECTOR]
    cmp     al,     SEC_PER_CYLINDER_PLUS_1
    jb      SETUP_CHS_OK
    sub     al,     18
    mov     [SECTOR],   al
    mov     al,     [HEAD]
    inc     al
    and     al,     1
    mov     [HEAD], al
    test    al,     al
    jnz     SETUP_CHS_OK
    inc byte    [CYLINDER]
SETUP_CHS_OK:
    mov     eax,    [DEST_ADDR]
    add     eax,    512
    mov     [DEST_ADDR],    eax

    inc byte    [LOADED_NUM]
    mov     al,     [LOADED_NUM]
    cmp     al,     SETUP_SEC_NUM
    jz      LOAD_SETUP_OK
    jmp     LOAD_SETUP_LOOP
LOAD_SETUP_OK:

;-----------load kernel sectors------------------------------------------------------------
    mov     al,     KERNEL_START_HEAD
    mov     [HEAD], al
    mov     al,     KERNEL_START_CYLINDER
    mov     [CYLINDER], al
    mov     al,     KERNEL_START_SEC
    mov     [SECTOR],   al
    xor     eax,    eax
    mov     ax,     KERNEL_SEG
    shl     eax,    4
    add     eax,    KERNEL_ADDR
    mov     [DEST_ADDR],    eax
    mov     eax,    0
    mov     [LOADED_NUM],   eax

LOAD_KERNEL_LOOP:
    mov     dl,     FLOOPPY_DRIVE       ;driver
    mov     dh,     [HEAD]              ;head
    mov     ch,     [CYLINDER]          ;cylinder
    mov     cl,     [SECTOR]            ;sector
    mov     eax,    [DEST_ADDR]
    and     eax,    000f0000h
    shr     eax,    4
    mov     es,     ax                  ;segment
    mov     eax,    [DEST_ADDR]
    mov     bx,     ax                  ;address
    mov     ah,     02h                 ;read sectors from drive
    mov     al,     01h                 ;sectors to read count
    int     13h

    inc byte    [SECTOR]
    mov     al,     [SECTOR]
    cmp     al,     SEC_PER_CYLINDER_PLUS_1
    jb      KERNEL_CHS_OK
    sub     al,     18
    mov     [SECTOR],   al
    mov     al,     [HEAD]
    inc     al
    and     al,     1
    mov     [HEAD], al
    test    al,     al
    jnz     KERNEL_CHS_OK
    inc byte    [CYLINDER]
KERNEL_CHS_OK:
    mov     eax,    [DEST_ADDR]
    add     eax,    512
    mov     [DEST_ADDR],    eax

    inc byte    [LOADED_NUM]
    mov     al,     [LOADED_NUM]
    cmp     al,     KERNEL_SEC_NUM
    jz      LOAD_KERNEL_OK
    jmp     LOAD_KERNEL_LOOP
LOAD_KERNEL_OK:


;--------------kill motor-------------------------------------------
    mov     dx,     03f2h
    mov     al,     0
    out     dx,     al

PRINT_BOOT_SUCCESS:
    ;get cursor position
    mov     ah,     03h
    xor     bh,     bh
    int     10h

    ;print message
    mov     cx,     MSG1_LEN
    mov     bx,     0007h
    mov     ax,     cs
    mov     es,     ax
    mov     bp,     MSG1
    mov     ax,     1301h
    int     10h
    mov     ah,     01h
    mov     cx,     0706h
    int     13h

    ;hide cursor
    mov     ah,     01h
    mov     cx,     0706h
    int     10h

    jmp     SETUP_SEG:SETUP_ADDR


;----------data-------------------------
HEAD:       db      0
CYLINDER:   db      0
SECTOR:     db      0
LOADED_NUM: dd      0
DEST_ADDR:  dd      0

MSG1:
    db      "boot success!"
    db      0dh,    0ah
MSG1_LEN    equ     $ - MSG1

BOOT_FLAG:
    times   510 - ($ - $$)  db  0
    db      55h,    0aah

BOOTSEC_SEG         equ     0000h
SETUP_SEG           equ     9000h
SYS_SEG             equ     5000h
STACK_SEG           equ     9000h
STACK_TOP           equ     0ff00h
SETUP_SEC_START     equ     2
SETUP_SEC_NUM       equ     3
SYS_SEC_START       equ     5
SYS_SEC_NUM         equ     14
SETUP_ADDR          equ     0000h
BLOCK_PER_SEG       equ     128
BLOCK_PER_CYLINDER  equ     18
CYLINDER_PER_HEAD   equ     80
HEAD_PER_FLOPPY     equ     2

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
    xor     al,     al
    





    jmp     SETUP_SEG:SETUP_ADDR


;---------------------------------------------------------------------------------------





    mov     esi,    SETUP_SEC_NUM
    mov     ax,     SETUP_SEG
    mov     es,     ax
    mov     bx,     0
    mov     dx,     0
    mov     cx,     SETUP_SEC_START
       
READ_SETUP_LOOP:
    test    esi,    esi
    jz      READ_SETUP_OVER
    xor     eax,    eax
    mov     al,     19
    sub     al,     cl
    cmp     eax,    esi
    jc      SETUP_SEC_NUM_IN_CYLINDER_LOW_THAN_REMAIN
    mov     ax,     si
SETUP_SEC_NUM_IN_CYLINDER_LOW_THAN_REMAIN:
    mov     di,     ax
    mov     ah,     02h
    int     13h
    add     bx,     512
    jnc     SETUP_SEGMENT_NOT_FULL
    mov     ax,     es
    add     ax,     1000h
    mov     es,     ax
SETUP_SEGMENT_NOT_FULL:
    add     cx,     di
    xor     ax,     ax
    mov     al,     cl
;    shl     al,     2
;    shr     al,     2
    and     al,     00111111b
    cmp     al,     20
    jc     SETUP_SECTOR_IN_CYLINDER_NOT_FINISHED
    add     cx,     0000000001000000b
    sub     cx,     18
    mov     ax,     cx
    shr     ax,     6
    cmp     ax,     81
    jc      SETUP_CYLINDER_IN_HEAD_NOT_FINISHED
    add     dh,     1
    sub     cx,     0001010000000000b
SETUP_CYLINDER_IN_HEAD_NOT_FINISHED:
SETUP_SECTOR_IN_CYLINDER_NOT_FINISHED:
    sub     si,     di
    jmp     READ_SETUP_LOOP
READ_SETUP_OVER:

;-------------copy sys-------------------------------------
    xor     eax,    eax
    xor     ebx,    ebx
    xor     ecx,    ecx
    xor     edx,    edx
    mov     esi,    SYS_SEC_NUM
    mov     ax,     SYS_SEG
    mov     es,     ax
    mov     bx,     0
    mov     dx,     0
    mov     cx,     SYS_SEC_START

READ_SYS_LOOP:
    test    esi,    esi
    jz      READ_SYS_OVER
    xor     eax,    eax
    mov     al,     19
    sub     al,     cl
    cmp     eax,    esi
    jc      SYS_SEC_NUM_IN_CYLINDER_LOW_THAN_REMAIN
    mov     ax,     si
SYS_SEC_NUM_IN_CYLINDER_LOW_THAN_REMAIN:
    mov     di,     ax
    mov     ah,     02h
    int     13h
    add     bx,     512
    jnc     SYS_SEGMENT_NOT_FULL
    mov     ax,     es
    add     ax,     1000h
    mov     es,     ax
SYS_SEGMENT_NOT_FULL:
    add     cx,     di
    xor     ax,     ax
    mov     al,     cl
;    shl     al,     2
;    shr     al,     2
    and     al,     00111111b
    cmp     al,     20
    jc     SYS_SECTOR_IN_CYLINDER_NOT_FINISHED
    add     cx,     0000000001000000b
    sub     cx,     18
    mov     ax,     cx
    shr     ax,     6
    cmp     ax,     81
    jc      SYS_CYLINDER_IN_HEAD_NOT_FINISHED
    add     dh,     1
    sub     cx,     0001010000000000b
SYS_CYLINDER_IN_HEAD_NOT_FINISHED:
SYS_SECTOR_IN_CYLINDER_NOT_FINISHED:
    sub     si,     di
    jmp     READ_SYS_LOOP
READ_SYS_OVER:
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

MSG1:
    db      "boot success!"
    db      0dh,    0ah
MSG1_LEN    equ     $ - MSG1

BOOT_FLAG:
    times   510 - ($ - $$)  db  0
    db      55h,    0aah

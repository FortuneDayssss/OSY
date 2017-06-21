%include "macro.inc"
org 00090000h
    jmp     SETUP16_ENTRY

[SECTION .memdata]
[BITS 32]
MEM_DATA:
    MCR_NUMBER  dd      0
    MEM_CHECK_BUFFER:
        times   256     db      0
    MEM_DATA_LEN    equ     $ - MEM_DATA

[SECTION .gdt]
[BITS 32]
LABEL_GDT:
    LABEL_DESC_DUMMY:   Descriptor  0,          0,          0
    LABEL_DESC_MEMC:    Descriptor  00000000h,  0fffffh,    DA_C + DA_32
    LABEL_DESC_MEMD:    Descriptor  00000000h,  0fffffh,    DA_DRW + DA_32
    LABEL_DESC_VIDEO:   Descriptor  000b8000h,  0ffffh,     DA_DRW
    LABEL_DESC_DATA:    Descriptor  00000000h,  0fffffh,    DA_DRW
GDT_LEN         equ     $ - LABEL_GDT
GDT_PTR         dw      GDT_LEN - 1
                dd      0
SELECTOR_MEMC   equ     LABEL_DESC_MEMC - LABEL_GDT
SELECTOR_MEMD   equ     LABEL_DESC_MEMD - LABEL_GDT
SELECTOR_VIDEO  equ     LABEL_DESC_VIDEO - LABEL_GDT
SELECTOR_DATA   equ     LABEL_DESC_DATA - LABEL_GDT


[SECTION .setup]
[BITS 16]
SETUP16_ENTRY:
    mov     ax,     9000h
    mov     es,     ax
    mov     ds,     ax
    mov     di,     MEM_CHECK_BUFFER
    mov     ebx,    0

CHECK_MEM_LOOP:
    mov     eax,    0e820h
    mov     ecx,    20
    mov     edx,    0534d4150h
    int     15h
    jc      CHECK_MEM_FAIL
    add     di,     20
    inc dword       [MCR_NUMBER]
    cmp     ebx,    0
    jne     CHECK_MEM_LOOP
    jmp     CHECK_MEM_SUCCESS
CHECK_MEM_FAIL:
    mov dword       [MCR_NUMBER],   0
CHECK_MEM_SUCCESS:
    xor     eax,    eax
    mov     ax,     cs
    shl     eax,    4
    add     ax,     MEM_DATA
    mov word    [LABEL_DESC_DATA + 2],  ax
    shr     eax,    16
    mov byte    [LABEL_DESC_DATA + 4],  al
    mov byte    [LABEL_DESC_DATA + 7],  ah

    xor     eax,    eax
    mov     ax,     ds
    shl     eax,    4
    add     ax,     LABEL_GDT
    mov dword   [GDT_PTR + 2],  eax
    lgdt    [GDT_PTR]

    cli

    ;open A20 bus
    in      al,     92h
    or      al,     00000010b
    out     92h,    al

    ;set cr0.PE = 1
    mov     eax,    cr0
    or      eax,    01b
    mov     cr0,    eax

    ;long jump
    jmp dword   SELECTOR_MEMC:SETUP32_ENTRY

[SECTION .stack32]
[BITS 32]
LABEL_STACK32:
    times 512   db      0
STACK32_TOP equ     $ - 1

[SECTION .setup32]
[BITS 32]
SETUP32_ENTRY:
    mov     ax,     SELECTOR_MEMD
    mov     ds,     ax
    mov     es,     ax
    mov     ss,     ax
    mov     esp,    STACK32_TOP
    mov     ax,     SELECTOR_VIDEO
    mov     gs,     ax

TEST_LOOP:
    jmp     TEST_LOOP










;test

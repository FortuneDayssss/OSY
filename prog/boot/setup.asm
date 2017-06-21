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
PAGE_DIR_BASE   equ     00200000h
PAGE_TBL_BASE   equ     00201000h

LABEL_GDT:
    LABEL_DESC_DUMMY:   Descriptor  0,              0,          0
    LABEL_DESC_MEMC:    Descriptor  00000000h,      0fffffh,    DA_C + DA_32
    LABEL_DESC_MEMD:    Descriptor  00000000h,      0fffffh,    DA_DRW + DA_32
    LABEL_DESC_VIDEO:   Descriptor  000b8000h,      0ffffh,     DA_DRW
    LABEL_DESC_DATA:    Descriptor  00000000h,      0fffffh,    DA_DRW
    LABEL_DESC_PDIR:    Descriptor  PAGE_DIR_BASE,  4096,       DA_DRW
    LABEL_DESC_PTBL:    Descriptor  PAGE_TBL_BASE,  1023,       DA_DRW | DA_LIMIT_4K
GDT_LEN         equ     $ - LABEL_GDT
GDT_PTR         dw      GDT_LEN - 1
                dd      0
SELECTOR_MEMC   equ     LABEL_DESC_MEMC - LABEL_GDT
SELECTOR_MEMD   equ     LABEL_DESC_MEMD - LABEL_GDT
SELECTOR_VIDEO  equ     LABEL_DESC_VIDEO - LABEL_GDT
SELECTOR_DATA   equ     LABEL_DESC_DATA - LABEL_GDT
SELECTOR_PDIR   equ     LABEL_DESC_PDIR - LABEL_GDT
SELECTOR_PTBL   equ     LABEL_DESC_PTBL - LABEL_GDT


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

    ;page
    mov     ax,     SELECTOR_PDIR
    mov     es,     ax
    xor     ebx,    ebx
    xor     eax,    eax
    mov     ecx,    1024
    mov     eax,    PAGE_TBL_BASE | PG_P | PG_USU | PG_RWW
INIT_PAGE_DIR_LOOP:
    mov     [es : ebx],  eax
    add     eax,    4096
    add     ebx,    4
    loop    INIT_PAGE_DIR_LOOP

    mov     ax,     SELECTOR_PTBL
    mov     es,     ax
    xor     ebx,    ebx
    xor     eax,    eax
    mov     eax,    0 | PG_P | PG_USU | PG_RWW
    mov     ecx,     1024 * 1024
INIT_PAGE_TBL_LOOP:
    mov     [es : ebx], eax
    add     eax,    4096
    add     ebx,    4
    loop    INIT_PAGE_TBL_LOOP

    mov     eax,    PAGE_DIR_BASE
    mov     cr3,    eax
    mov     eax,    cr0
    or      eax,    80000000h
    mov     cr0,    eax
    jmp     PAGING_OK
PAGING_OK:

TEST_LOOP:
    jmp     TEST_LOOP










;test

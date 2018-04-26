BOOTSEC_SEG             equ     0000h
SETUP_SEG               equ     9000h
KERNEL_SEG              equ     5000h
STACK_SEG               equ     9000h
STACK_TOP               equ     0ff00h

SETUP_SEC_NR            equ     3
KERNEL_SEC_NR           equ     0ffh

BUFFER_SEG              equ     1000h
BUFFER_ADDR             equ     0000h

SUPER_BLOCK_LBA                 equ     1
SUPER_BLOCK_ROOT_INODE_OFFSET   equ     28
SUPER_BLOCK_IMAP_SEC_NR_OFFSET  equ     12
SUPER_BLOCK_SMAP_SEC_NR_OFFSET  equ     16
SUPER_BLOCK_ROOT_SECS_OFFSET    equ     20

DIR_ENTRY_LEN                   equ     64
DIR_ENTRY_FILENAME_OFFSET       equ     4
ROOT_DIR_ENTRY_LIST_LEN         equ     32      ;max length in 4 sector
ROOT_DIR_ENTRY_LIST_SEC_NR      equ     4       ;default sector number = 4

INODE_LEN                       equ     32
INODE_START_SEC_OFFSET          equ     8
INODE_NR_SEC_OFFSET             equ     12


KERNEL_ADDR             equ     0000h           ;00050000 => kernel elf
SETUP_ADDR              equ     0000h           ;00090000 => setup

FS_OSY_MAGIC_V1         equ     0bbah


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
    mov     ax,     cs
    mov     ds,     ax
    mov     es,     ax
    mov     ax,     BUFFER_SEG
    mov     fs,     ax

;--------load super block-----------------------------------------------------------
    xor     ebx,    ebx
    mov byte    [ds : PacketLen],   10h
    mov byte    [ds : BlockNum],    1
    mov word    [ds : DstAddr],     BUFFER_ADDR
    mov word    [ds : DstSeg],      BUFFER_SEG
    mov dword   [ds : LBALow],      SUPER_BLOCK_LBA
    call    read_sector

    mov     eax,    [fs : BUFFER_ADDR]
    cmp     eax,    FS_OSY_MAGIC_V1
    jz      FS_EXIST
    call    error
FS_EXIST:
    xor     eax,    eax
    add     eax,    2       ; boot sector, super block
    add     eax,    [fs : BUFFER_ADDR + SUPER_BLOCK_IMAP_SEC_NR_OFFSET]
    add     eax,    [fs : BUFFER_ADDR + SUPER_BLOCK_SMAP_SEC_NR_OFFSET]
    mov dword   [ds : INodeListStartSectorNr],  eax

    ; load root sectors (default num = 4) to 0x00010000
    mov     eax,    [fs : BUFFER_ADDR + SUPER_BLOCK_ROOT_SECS_OFFSET]
    mov byte    [ds : BlockNum],    ROOT_DIR_ENTRY_LIST_SEC_NR
    mov dword   [ds : LBALow],      eax
    call    read_sector

    ; find setup file
    mov     ax,     BUFFER_SEG
    mov     ds,     ax
    mov     ax,     cs
    mov     es,     ax
    mov     bx,     SetupFileName
    mov     di,     SetupFileNameLen
    mov     si,     DIR_ENTRY_FILENAME_OFFSET
    mov     cx,     ROOT_DIR_ENTRY_LIST_LEN
find_setup_loop:
    call    filename_strcmp
    test    eax,    eax
    jnz     find_setup_ok
    add     si,     DIR_ENTRY_LEN
    loop    find_setup_loop
    mov     ax,     cx
    mov     ds,     ax
    mov     es,     ax
    call    error
find_setup_ok:
    sub     si,     DIR_ENTRY_FILENAME_OFFSET
    mov     eax,    [ds : si]
    mov     [es : SetupINodeIndex],     eax

;---------find kernel--------------------------------------------
    mov     bx,     KernelFileName
    mov     di,     KernelFileNameLen
    mov     si,     DIR_ENTRY_FILENAME_OFFSET
    mov     cx,     ROOT_DIR_ENTRY_LIST_LEN
find_kernel_loop:
    call    filename_strcmp
    test    eax,    eax
    jnz     find_kernel_ok
    add     si,     DIR_ENTRY_LEN
    loop    find_kernel_loop
    mov     ax,     cx
    mov     ds,     ax
    mov     es,     ax
    call    error
find_kernel_ok:
    sub     si,     DIR_ENTRY_FILENAME_OFFSET
    mov     eax,    [ds : si]
    mov     [es : KernelINodeIndex],    eax

;---------load inode list---------------------------------------
    mov     ax,     cs
    mov     ds,     ax
    mov     es,     ax
    mov     eax,    [ds : INodeListStartSectorNr]
    mov byte    [ds : BlockNum],    4
    mov dword   [ds : LBALow],      eax
    call    read_sector
;---------load setup--------------------------------------------
    xor     eax,    eax
    mov     ecx,    [ds : SetupINodeIndex]
    dec     ecx                             ; inode id start from 1

    mov     eax,    INODE_LEN
    mul     cx

    mov     ebx,    eax
    add     ebx,    INODE_START_SEC_OFFSET
    mov     edx,    [fs : bx]

    mov dword   [ds : LBALow],          edx
    mov byte    [ds : BlockNum],        SETUP_SEC_NR
    mov word    [ds : DstSeg],          SETUP_SEG
    mov word    [ds : DstAddr],         SETUP_ADDR
    call    read_sector

;---------load kernel-------------------------------------------
    xor     eax,    eax
    mov     ecx,    [ds : KernelINodeIndex]
    dec     ecx                             ; inode id start from 1

    mov     eax,    INODE_LEN
    mul     cx

    mov     ebx,    eax
    add     ebx,    INODE_START_SEC_OFFSET
    mov     edx,    [fs : bx]

    mov dword   [ds : LBALow],          edx
    mov byte    [ds : BlockNum],        KERNEL_SEC_NR
    mov word    [ds : DstSeg],          KERNEL_SEG
    mov word    [ds : DstAddr],         KERNEL_ADDR
    call    read_sector

    
BOOT_SUCCESS:
    ;hide cursor
    mov     ah,     01h
    mov     cx,     0706h
    int     10h

    jmp     SETUP_SEG:SETUP_ADDR

;----------function---------------------------------------------
filename_strcmp: ;(es:bx, ds:si, len=di)
    push    dx
    push    cx
    push    bx
    push    si
    mov     cx,     di
strcmp_loop:
    mov     al,     [es : bx]
    mov     dl,     [ds : si]
    cmp     al,     dl
    jnz     strcmp_false
    add     bx,     1
    add     si,     1
    loop    strcmp_loop
    pop     si
    pop     bx
    pop     cx
    pop     dx
    mov     eax,    1
    ret
strcmp_false:
    pop     si
    pop     bx
    pop     cx
    pop     dx
    mov     eax,    0
    ret

read_sector:
    xor     ebx,    ebx
    mov     ah,     42h     ;EDD
    mov     dl,     80h     ;drive number
    mov     si,     DiskAddressPacket
    int     13h
    ret

error:
ERROR_LOOP:
    jmp     ERROR_LOOP

;----------data--------------------------------------------------
DiskAddressPacket   equ     8c00h
    PacketLen       equ     DiskAddressPacket
    Reserved0       equ     PacketLen + 1
    BlockNum        equ     Reserved0 + 1
    Reserved1       equ     BlockNum + 1
    DstAddr         equ     Reserved1 + 1
    DstSeg          equ     DstAddr + 2
    LBALow          equ     DstSeg + 2
    LBAHigh         equ     LBALow + 4

PrintStringVariable:
    StringAddr: dw  0
    StringLen:  dw  0

INodeListStartSectorNr  dd  0
SetupINodeIndex         dd  0
KernelINodeIndex        dd  0

SetupFileName       db  "setup.bin", 0
SetupFileNameLen    equ 10
KernelFileName      db  "kernel.bin", 0
KernelFileNameLen   equ 11


BOOT_FLAG:
    times   510 - ($ - $$)  db  0
    db      55h,    0aah
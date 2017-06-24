%include "macro.inc"
KERNEL_ELF_ADDR equ     00050000h
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
    ;init segment register for protect mode
    mov     ax,     SELECTOR_MEMD
    mov     ds,     ax
    mov     es,     ax
    mov     ss,     ax
    mov     esp,    STACK32_TOP
    mov     ebp,    esp
    mov     ax,     SELECTOR_VIDEO
    mov     gs,     ax

    ;load kernel code from elf to correct addr
    mov     ebx,    KERNEL_ELF_ADDR
    add     ebx,    42
    xor     eax,    eax
    mov     ax,     [ds : ebx]              ;elfHeader + 42 = elfHeader->e_phentsize(2Byte)
    push    eax                             ;programHeaderSize [ss : ebp - 4]

    mov	    ebx,    KERNEL_ELF_ADDR
    add	    ebx,    28
    mov	    eax,    [ds : ebx]              ;elfHeader + 28 = elfHeader->e_phoff(4Byte)
    mov	    ebx,    KERNEL_ELF_ADDR
    add	    ebx,    eax
    push    ebx                             ;programHeaderAddress [ss : ebp - 8]

    mov     ebx,    KERNEL_ELF_ADDR
    add     ebx,    46
    xor     eax,    eax
    mov     ax,     [ds : ebx]              ;elfHeader + 46 = elfHeader->e_shentsize(2Byte)
    push    eax                             ;sectionHeaderSize [ss : ebp - 12]

    mov		ebx,    KERNEL_ELF_ADDR
    add		ebx,    32
    mov		eax,    [ds : ebx]              ;elfHeader + 32 = elfHeader->e_shoff(4Byte)
    mov		ebx,    KERNEL_ELF_ADDR
    add		ebx,    eax
    push    ebx                             ;sectionHeaderAddress [ss : ebp - 16]

    mov     ebx,    KERNEL_ELF_ADDR
    add     ebx,    44
    xor     ecx,    ecx
    mov     cx,     [ds : ebx]              ;elfHeader + 44 = elfHeader->e_phnum(2Byte)
    dec     cx                              ;???last program header always size == 0???

;---------------------------------------------------------------------------------------------
Load_Kernel_Program_Loop:
    mov     ebx,    [ebp - 8]               ;ebx <= &(first program header)
    mov     eax,    [ds : ebx + 20]         ;prgmHeader + 20 = prgmHeader->p_memsz(4Byte)
    push    eax                             ;size

    mov     eax,    [ds : ebx + 8]          ;prgmHeader + 8 = prgmHeader->p_vaddr(4Byte)
    push    eax                             ;opAddress

    mov     eax,    [ds : ebx + 4]
    mov     ebx,    KERNEL_ELF_ADDR
    add     ebx,    eax
    push    ebx                             ;rsrcAddress

    call    ASM_Memcpy
    add     esp,    12

	;prgmHeader = prgmHeader + prgmHeaderSize(= nextPrgmHeader)
    mov     ebx,    [ebp - 8]
    mov     eax,    [ebp - 4];
    add     ebx,    eax
    mov     [ebp - 8],  ebx
	
	loop    Load_Kernel_Program_Loop

    mov     ebx,    KERNEL_ELF_ADDR
    add     ebx,    48
    xor     ecx,    ecx
    mov     cx,     [ds : ebx]              ;elfHeader + 44 = elfHeader->e_shnum(2Byte)
Load_Kernel_Section_Loop:
	mov		ebx,		[ebp - 16];ebx <= &(first section header)

	;if(sectionHeader->sh_type == SHT_NOBIT(00000008))memset0(section, sectionLength);	
    mov     eax,    [ds : ebx + 4]          ;sectionHeader + 4 = sectionHeader->sh_type(4Byte)
	cmp     eax,    00000008h
	jnz     ISNT_NOBITS
	mov     eax,    [ds : ebx + 20]
	push    eax
	mov     eax,    [ds : ebx + 12]
	push    eax
	call    ASM_Memset0
	add     esp,    8
	jmp     Load_Kernel_Section_Continue

ISNT_NOBITS:
    ;if(sectionHeader->sh_addr == 0)continue;
    mov     eax,    [ds : ebx + 12]
    cmp     eax,    0
    jz      Load_Kernel_Section_Continue

    mov     eax,    [ds : ebx + 20]         ;sectHeader + 20 = sectHeader->sh_size(4Byte)
    push    eax;size

    mov     eax,    [ds : ebx + 12]         ;sectHeader + 12 = sectHeader->sh_addr(4Byte)
	push    eax;opAddress

    mov     eax,    [ds : ebx + 16]
    mov     ebx,    KERNEL_ELF_ADDR
    add     ebx,    eax
    push    ebx                             ;rsrcAddress

    call    ASM_Memcpy
    add     esp,    12

Load_Kernel_Section_Continue:

    ;sectHeader = sectHeader + sectHeaderSize(= nextSectHeader)
    mov     ebx,    [ebp - 16]
    mov     eax,    [ebp - 12];
    add     ebx,    eax
    mov     [ebp - 16], ebx
	
    loop    Load_Kernel_Section_Loop

;---------------------------------------------------------------------------------------------

    ;paging
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

;jump to kernel code
    jmp     00010000h


;-------------------function--------------------------------------------
;void ASM_Memcpy(byte4 rsrcAddr, byte4 opAddr, byte4 size)
;ds:rsrcAddr => es:opAddr
ASM_Memcpy:
	push		ebp
	mov		ebp,		esp
	push		eax
	push		ecx
	push		esi
	push		edi

	mov		esi,		[ebp + 8]	;rsrcAddr
	mov		edi,		[ebp + 12]	;opAddr
	mov		ecx,		[ebp + 16]	;size
	shr		ecx,		1
	add		ecx,		1
MEMCOPY_LOOP:
	mov		eax,		[ds : esi]
	mov		[ds : edi],	eax
	add		esi,		4
	add		edi,		4		
	loop		MEMCOPY_LOOP

	pop		edi
	pop		esi
	pop		ecx
	pop		eax
	pop		ebp
	ret
;void memset0(u32 opAddr, u32 length)
ASM_Memset0:
	push		ebp
	mov		ebp,		esp
	push		eax
	push		ecx
	push		esi

	mov		esi,		[ebp + 8]	;opAddr
	mov		ecx,		[ebp + 12]	;length
	add		ecx,		1
	mov		al,		0
Memset_LOOP:
	mov		[ds : esi],	al
	add		esi,		1		
	loop		Memset_LOOP

	pop		esi
	pop		ecx
	pop		eax
	pop		ebp
	ret
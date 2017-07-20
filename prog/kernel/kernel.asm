extern gdt_ptr
extern idt_ptr
extern memdata_ptr
extern main
extern init_kernel
extern exception_handler
extern dummy_irq
extern irq_table
extern test
extern schedule_output_test
extern get_next_process

extern current_process
extern next_process
extern tss

global _start
global change_to_user_mode

;exception
global divide_error
global single_step_exception
global nmi
global breakpoint_exception
global overflow
global bounds_check
global inval_opcode
global copr_not_available
global double_fault
global copr_seg_overrun
global inval_tss
global segment_not_present
global stack_exception
global general_protection
global page_fault
global copr_error

;hardware interrupt
global	hwint00
global	hwint01
global	hwint02
global	hwint03
global	hwint04
global	hwint05
global	hwint06
global	hwint07
global	hwint08
global	hwint09
global	hwint10
global	hwint11
global	hwint12
global	hwint13
global	hwint14
global	hwint15

%include "asm/macro.inc"
[section .bss]
StackSpace  resb    4*1024
StackTop:

[section .text]
[bits 32]
_start:
    pop     eax
    mov     [memdata_ptr],  eax
    mov     esp,    StackTop
    sgdt    [gdt_ptr]
    call    init_kernel
    lgdt    [gdt_ptr]
    lidt    [idt_ptr]
    jmp     SELECTOR_MEMC_0:NEW_GDT_OK
NEW_GDT_OK:
    xor     eax,    eax
    mov     ax,     SELECTOR_TSS
    ltr     ax
    jmp     main



;-------exception-------------------------------------------------------------
divide_error:
	push	0xFFFFFFFF  ; no err code
	push	0           ; vector_no	= 0
	jmp	exception
single_step_exception:
	push	0xFFFFFFFF  ; no err code
	push	1           ; vector_no	= 1
	jmp	exception
nmi:
	push	0xFFFFFFFF  ; no err code
	push	2           ; vector_no	= 2
	jmp	exception
breakpoint_exception:
	push	0xFFFFFFFF  ; no err code
	push	3           ; vector_no	= 3
	jmp	exception
overflow:
	push	0xFFFFFFFF  ; no err code
	push	4           ; vector_no	= 4
	jmp	exception
bounds_check:
	push	0xFFFFFFFF  ; no err code
	push	5           ; vector_no	= 5
	jmp	exception
inval_opcode:
	push	0xFFFFFFFF  ; no err code
	push	6           ; vector_no	= 6
	jmp	exception
copr_not_available:
	push	0xFFFFFFFF  ; no err code
	push	7           ; vector_no	= 7
	jmp	exception
double_fault:
	push	8           ; vector_no	= 8
	jmp	exception
copr_seg_overrun:
	push	0xFFFFFFFF  ; no err code
	push	9           ; vector_no	= 9
	jmp	exception
inval_tss:
	push	10          ; vector_no	= A
	jmp	exception
segment_not_present:
	push	11          ; vector_no	= B
	jmp	exception
stack_exception:
	push	12          ; vector_no	= C
	jmp	exception
general_protection:
	push	13          ; vector_no	= D
	jmp	exception
page_fault:
	push	14          ; vector_no	= E
	jmp	exception
copr_error:
	push	0xFFFFFFFF  ; no err code
	push	16          ; vector_no	= 10h
	jmp	exception

exception:
	call    exception_handler
	add     esp,    4*2
	hlt

;------hardware interrupt-------------------------------------------------------
%macro	before_irq_handler	1
	pushad
	in		al,		INT_M_CTLMASK
	or		al,		(1 << %1)
	out		INT_M_CTLMASK,	al
	mov		al,		EOI
	out		INT_M_CTL,	al
	mov		ax,		ss
	mov		ds,		ax
	mov		es,		ax
	mov		fs,		ax
	sti
%endmacro

%macro	after_irq_handler	1
	cli
	in		al,		INT_M_CTLMASK
	and		al,		~(1 << %1)
	out		INT_M_CTLMASK,	al
	mov		ax,		SELECTOR_MEMD_3
	mov		ds,		ax
	mov		es,		ax
	mov		fs,		ax
	mov		ax,		SELECTOR_VIDEO
	mov		gs,		ax
	popad
	iretd
%endmacro


ALIGN	16
hwint00:		; Interrupt routine for irq 0 (the clock).
	before_irq_handler	0
	mov		ebx,	[current_process]
	mov	dword	[ebx + PCB_OFFSET_ESP],	esp
	call	get_next_process
	mov		eax,	[current_process]
	mov		ebx,	[next_process]
	cmp		eax,	ebx
	jz		SCHEDULE_OK

	pushad											;-----
	call	schedule_output_test					;test output "schedule!"
	popad											;-----
	mov		[current_process],	ebx
	mov		esp,	[current_process]
	lea		eax,	[esp + PCB_OFFSET_STACK0TOP]
	mov		[tss + TSS_OFFSET_SP0],	eax
	lea		eax,	[esp + PCB_OFFSET_STACK1TOP]
	mov		[tss + TSS_OFFSET_SP1],	eax
	lea		eax,	[esp + PCB_OFFSET_STACK2TOP]
	mov		[tss + TSS_OFFSET_SP2],	eax
	lea		eax,	[esp + PCB_OFFSET_ESP]
	mov		esp,	[eax]
SCHEDULE_OK:
	after_irq_handler	0

ALIGN	16
hwint01:		; Interrupt routine for irq 1 (keyboard)
	before_irq_handler	1
	call	test
	in		al,		60h
	or		al,		80h
	out		60h,	al
	after_irq_handler	1

ALIGN	16
hwint02:		; Interrupt routine for irq 2 (cascade!)
	before_irq_handler	2
	after_irq_handler	2

ALIGN	16
hwint03:		; Interrupt routine for irq 3 (second serial)
	before_irq_handler	3
	after_irq_handler	3

ALIGN	16
hwint04:		; Interrupt routine for irq 4 (first serial)
	before_irq_handler	4
	after_irq_handler	4

ALIGN	16
hwint05:		; Interrupt routine for irq 5 (XT winchester)
	before_irq_handler	5
	after_irq_handler	5

ALIGN	16
hwint06:		; Interrupt routine for irq 6 (floppy)
	before_irq_handler	6
	after_irq_handler	6

ALIGN	16
hwint07:		; Interrupt routine for irq 7 (printer)
	before_irq_handler	7
	after_irq_handler	7

; ---------------------------------
%macro	hwint_slave	1
	push	%1
	call	dummy_irq
	add	esp, 4
	hlt
%endmacro
; ---------------------------------

ALIGN	16
hwint08:		; Interrupt routine for irq 8 (realtime clock).
	hwint_slave	8

ALIGN	16
hwint09:		; Interrupt routine for irq 9 (irq 2 redirected)
	hwint_slave	9

ALIGN	16
hwint10:		; Interrupt routine for irq 10
	hwint_slave	10

ALIGN	16
hwint11:		; Interrupt routine for irq 11
	hwint_slave	11

ALIGN	16
hwint12:		; Interrupt routine for irq 12
	hwint_slave	12

ALIGN	16
hwint13:		; Interrupt routine for irq 13 (FPU exception)
	hwint_slave	13

ALIGN	16
hwint14:		; Interrupt routine for irq 14 (AT winchester)
	hwint_slave	14

ALIGN	16
hwint15:		; Interrupt routine for irq 15
	hwint_slave	15

change_to_user_mode:
	mov		esp,	[current_process]
	lea		eax,	[esp + PCB_OFFSET_STACK0TOP]
	mov		[tss + TSS_OFFSET_SP0],	eax
	lea		eax,	[esp + PCB_OFFSET_STACK1TOP]
	mov		[tss + TSS_OFFSET_SP1],	eax
	lea		eax,	[esp + PCB_OFFSET_STACK2TOP]
	mov		[tss + TSS_OFFSET_SP2],	eax
	lea		eax,	[esp + PCB_OFFSET_ESP]
	mov		esp,	[eax]
	mov		ax,		SELECTOR_MEMD_3
	mov		ds,		ax
	mov		es,		ax
	mov		fs,		ax
	mov		ax,		SELECTOR_VIDEO
	mov		gs,		ax
	xor		edi,	edi
	xor		esi,	esi
	xor		ebx,	ebx
	xor		edx,	edx
	xor		ecx,	ecx
	xor		eax,	eax
	popad
	sti
	iretd

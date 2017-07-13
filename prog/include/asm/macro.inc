;GDT SELECTOR
SELECTOR_DUMMY      equ     0x00
SELECTOR_MEMC       equ     0x08
SELECTOR_MEMD       equ     0x10
SELECTOR_VIDEO      equ     0x18 + 3
SELECTOR_TSS        equ     0x20

;8259A interrupt controller ports.
INT_M_CTL           equ     0x20	; I/O port for interrupt controller         <Master>
INT_M_CTLMASK       equ     0x21	; setting bits in this port disables ints   <Master>
INT_S_CTL           equ     0xA0	; I/O port for second interrupt controller  <Slave>
INT_S_CTLMASK       equ     0xA1	; setting bits in this port disables ints   <Slave>

EOI		            equ	0x20

;PCB offset
PCB_OFFSET_GS           equ     0
PCB_OFFSET_FS           equ     PCB_OFFSET_GS + 4
PCB_OFFSET_ES           equ     PCB_OFFSET_FS + 4
PCB_OFFSET_DS           equ     PCB_OFFSET_ES + 4
PCB_OFFSET_EDI          equ     PCB_OFFSET_DS + 4
PCB_OFFSET_ESI          equ     PCB_OFFSET_EDI + 4
PCB_OFFSET_EBP          equ     PCB_OFFSET_ESI + 4
PCB_OFFSET_KEBP         equ     PCB_OFFSET_EBP + 4
PCB_OFFSET_EBX          equ     PCB_OFFSET_KEBP + 4
PCB_OFFSET_EDX          equ     PCB_OFFSET_EBX + 4
PCB_OFFSET_ECX          equ     PCB_OFFSET_EDX + 4
PCB_OFFSET_EAX          equ     PCB_OFFSET_ECX + 4
PCB_OFFSET_EIP          equ     PCB_OFFSET_EAX + 4
PCB_OFFSET_CS           equ     PCB_OFFSET_EIP + 4
PCB_OFFSET_EFLAGS       equ     PCB_OFFSET_CS + 4
PCB_OFFSET_ESP          equ     PCB_OFFSET_EFLAGS + 4
PCB_OFFSET_SS           equ     PCB_OFFSET_ESP + 4
PCB_OFFSET_RETADDR      equ     PCB_OFFSET_SS + 4
PCB_OFFSET_LDT_SELECTOR equ     PCB_OFFSET_RETADDR + 4
PCB_OFFSET_LDT          equ     PCB_OFFSET_LDT_SELECTOR + 2
PCB_OFFSET_STACK0       equ     PCB_OFFSET_LDT + 24                 ;size of Descriptor = 64 bit
PCB_OFFSET_STACK0TOP    equ     PCB_OFFSET_STACK0 + (4 * 1024) - 4
PCB_OFFSET_STACK1       equ     PCB_OFFSET_STACK0 + 4 * 1024        ;stacksize = 4k
PCB_OFFSET_STACK1TOP    equ     PCB_OFFSET_STACK1 + (4 * 1024) - 4
PCB_OFFSET_STACK2       equ     PCB_OFFSET_STACK1 + 4 * 1024
PCB_OFFSET_STACK2TOP    equ     PCB_OFFSET_STACK2 + (4 * 1024) - 4


;TSS offset
TSS_OFFSET_SP0          equ     0x04
TSS_OFFSET_SP1          equ     0x0c
TSS_OFFSET_SP2          equ     0x14

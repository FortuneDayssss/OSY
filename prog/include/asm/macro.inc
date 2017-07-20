;GDT SELECTOR
SELECTOR_DUMMY      equ     0x00
SELECTOR_MEMC_0     equ     0x08
SELECTOR_MEMD_0     equ     0x10
SELECTOR_MEMC_1     equ     0x18 + 1
SELECTOR_MEMD_1     equ     0x20 + 1
SELECTOR_MEMC_2     equ     0x28 + 2
SELECTOR_MEMD_2     equ     0x30 + 2
SELECTOR_MEMC_3     equ     0x38 + 3
SELECTOR_MEMD_3     equ     0x40 + 3
SELECTOR_VIDEO      equ     0x48 + 3
SELECTOR_TSS        equ     0x50

;8259A interrupt controller ports.
INT_M_CTL           equ     0x20	; I/O port for interrupt controller         <Master>
INT_M_CTLMASK       equ     0x21	; setting bits in this port disables ints   <Master>
INT_S_CTL           equ     0xA0	; I/O port for second interrupt controller  <Slave>
INT_S_CTLMASK       equ     0xA1	; setting bits in this port disables ints   <Slave>

EOI		            equ	0x20

;PCB offset
PCB_OFFSET_STACK0       equ     0
PCB_OFFSET_STACK0TOP    equ     PCB_OFFSET_STACK0 + (4 * 1024) - 4
PCB_OFFSET_STACK1       equ     PCB_OFFSET_STACK0 + 4 * 1024        ;stacksize = 4k
PCB_OFFSET_STACK1TOP    equ     PCB_OFFSET_STACK1 + (4 * 1024) - 4
PCB_OFFSET_STACK2       equ     PCB_OFFSET_STACK1 + 4 * 1024
PCB_OFFSET_STACK2TOP    equ     PCB_OFFSET_STACK2 + (4 * 1024) - 4
PCB_OFFSET_STACK3       equ     PCB_OFFSET_STACK2 + 4 * 1024
PCB_OFFSET_STACK3TOP    equ     PCB_OFFSET_STACK3 + (4 * 1024) - 4
PCB_OFFSET_ESP          equ     PCB_OFFSET_STACK3 + 4 * 1024


;TSS offset
TSS_OFFSET_SP0          equ     0x04
TSS_OFFSET_SP1          equ     0x0c
TSS_OFFSET_SP2          equ     0x14

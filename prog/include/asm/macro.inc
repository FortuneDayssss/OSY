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
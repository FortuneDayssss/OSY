;GDT SELECTOR
SELECTOR_DUMMY      equ     0x00
SELECTOR_MEMC       equ     0x08
SELECTOR_MEMD       equ     0x10
SELECTOR_VIDEO      equ     0x18 + 3
SELECTOR_TSS        equ     0x20
SELECTOR_KERNEL_CS  equ     SELECTOR_MEMC

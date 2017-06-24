extern gdt_ptr
extern main
extern init_kernel

global _start

[section .bss]
StackSpace  resb    4*1024
StackTop:

[section .text]
[bits 32]
_start:
    mov     esp,    StackTop
    sgdt    [gdt_ptr]
    call    init_kernel
    ;todo

test_loop:
    jmp     test_loop
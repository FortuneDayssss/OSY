extern main

[section .text]
[bits 32]
global _start

_start:
    call    main
test_loop:
    jmp     test_loop
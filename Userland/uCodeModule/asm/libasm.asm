GLOBAL invalidOpcode
GLOBAL divideZero

section .text

divideZero:
    mov rax, 0
    div rbx
    ret

invalidOpcode:
    db      0xFF, 0xFF
    ret


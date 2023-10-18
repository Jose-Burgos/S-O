GLOBAL loader
EXTERN main
EXTERN initializeKernelBinary
GLOBAL loadMain

EXTERN getStackBase


loader:
	call initializeKernelBinary     ; Set up the kernel binary, and get thet stack address
        jmp stack_set
loadMain:
        call getStackBase
stack_set:
        mov rsp, rax                    ; Set up the stack with the returned address
	call main
hang:
	cli
	hlt	; halt machine should kernel return
	jmp hang

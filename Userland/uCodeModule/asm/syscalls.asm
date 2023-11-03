GLOBAL sys_read
GLOBAL sys_write
GLOBAL sys_writeAt
GLOBAL sys_clearScreen
GLOBAL sys_wait
GLOBAL sys_time
GLOBAL sys_date
GLOBAL sys_getScreenHeight
GLOBAL sys_getScreenWidth
GLOBAL sys_timedRead
GLOBAL sys_drawRectangle
GLOBAL sys_changeFontSize
GLOBAL sys_inforeg
GLOBAL sys_beep
GLOBAL sys_malloc
GLOBAL sys_free
GLOBAL sys_memorystatus
GLOBAL sys_exec
GLOBAL sys_kill_process
GLOBAL sys_block_process
GLOBAL sys_process_ready
GLOBAL sys_get_pid
GLOBAL sys_wait_pid

; syscall 0x00
sys_write:
    mov rax, 0x00
    mov r10, rcx
    int 0x80
    ret

; syscall 0x01
sys_read:
    mov rax, 0x01
    int 0x80
    ret

; syscall 0x02
sys_writeAt:
    mov rax, 0x02
    mov r10, rcx
    int 0x80
    ret

; syscall 0x03
sys_clearScreen:
    mov rax, 0x03
    int 0x80
    ret

; syscall 0x04
sys_wait:
    mov rax, 0x04
    int 0x80
    ret

; syscall 0x05
sys_time:
    mov rax, 0x05
    int 0x80
    ret

; syscall 0x06
sys_date:
    mov rax, 0x06
    int 0x80
    ret

; syscall 0x07
sys_getScreenHeight:
    mov rax, 0x07
    int 0x80
    ret

; syscall 0x08
sys_getScreenWidth:
    mov rax, 0x08
    int 0x80
    ret

; syscall 0x09
sys_timedRead:
    mov rax, 0x09
    mov r10, rcx
    int 0x80
    ret

; syscall 0x0A
sys_drawRectangle:
    mov rax, 0x0A
    mov r10, rcx
    int 0x80
    ret

; syscall 0x0B
sys_changeFontSize:
    mov rax, 0x0B
    int 0x80
    ret

; syscall 0x0C
sys_inforeg:
    mov rax, 0x0C
    mov r10, rcx
    int 0x80
    ret

; syscall 0x0D
sys_beep:
    mov rax, 0x0D
    int 0x80
    ret

; syscall 0x0E
sys_malloc:
    mov rax, 0x0E
    int 0x80
    ret

; syscall 0x0F
sys_free:
    mov rax, 0x0F
    int 0x80
    ret

; syscall 0x10
sys_memorystatus:
    mov rax, 0x10
    mov r10, rcx
    int 0x80
    ret

; syscall 0x11
sys_exec:
    mov r11, [rsp + 0x08]
    mov rax, 0x11
    int 0x80
    ret

; syscall 0x12
sys_kill_process:
    mov rax, 0x12
    mov r10, rcx
    int 0x80
    ret

; syscall 0x13
sys_block_process:
    mov rax, 0x13
    mov r10, rcx
    int 0x80
    ret

; syscall 0x14
sys_process_ready:
    mov rax, 0x14
    mov r10, rcx
    int 0x80
    ret

; syscall 0x15
sys_get_pid:
    mov rax, 0x15
    ;mov r10, rcx
    int 0x80
    ret

; syscall 0x16
sys_wait_pid:
    mov rax, 0x16
    mov r10, rcx
    int 0x80
    ret
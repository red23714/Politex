    .text
    .global sys_write
sys_write:
    mov $1, %rax         # syscall: write
    syscall
    ret
    
    .section .note.GNU-stack,"",@progbits

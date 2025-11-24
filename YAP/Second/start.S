    .global _start
    .extern kmain
_start:
    call kmain           
    mov %rax, %rdi       
    mov $60, %rax        
    syscall

    .section .note.GNU-stack,"",@progbits

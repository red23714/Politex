    .global _start
    .extern kmain
_start:
    call kmain           # вызвать C-функцию kmain()
    mov %rax, %rdi       # вернуть код возврата из kmain
    mov $60, %rax        # syscall: exit
    syscall

    .section .note.GNU-stack,"",@progbits

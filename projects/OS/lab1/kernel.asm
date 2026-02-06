use32
org 0x10000

mov edi, 0xb8000
mov esi, str_hello
call video_puts

infinite_loop:
    hlt
    jmp infinite_loop

video_puts:
    mov al, [esi]
    test al, al
    jz video_puts_end

    mov ah, 0x07
    mov [edi], al
    mov [edi+1], ah

    add edi, 2
    add esi, 1
    jmp video_puts

video_puts_end:
    ret

str_hello:
    db "Zeig Heil and welcome to CumOs", 0

times (512 - ($ - $$)) db 0 

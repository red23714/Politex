use32
org 0x1000

kernel_start:
    ; СНАЧАЛА очищаем экран
    call clear_screen
 
    ; ПОТОМ выводим
    mov edi, 0xB8000
    mov esi, str_hello
    call video_puts
    
    jmp infinite_loop

clear_screen:
    pusha
    mov edi, 0xB8000
    mov ecx, 80*25      ; 80x25 символов
    mov ax, 0x0720      ; пробел + атрибут 0x07
    rep stosw
    popa
    ret

infinite_loop:
    ; Перевод процессора в бесконечный цикл
    hlt
    jmp infinite_loop

video_puts:
    ; Функция выводит в буфер видеопамяти (передается в edi) строку, оканчивающуюся 0 (передается в esi)
    ; После завершения edi содержит адрес по которому можно продолжать вывод следующих строк
    mov al, [esi]
    test al, al
    jz video_puts_end

    mov ah, 0x07 ; Цвет символа и фона. Возможные варианты: 0x00 is black-on-black, 0x07 is lightgrey-on-black, 0x1F is white-on-blue
    mov [edi], al
    mov [edi+1], ah

    add edi, 2
    add esi, 1

    jmp video_puts

video_puts_end:
    ret

str_hello:
    db "Zeig Heil welcome to CumOS (asm edition)!", 0

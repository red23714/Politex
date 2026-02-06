use16
org 0x7C00

start:
    ; Инициализация сегментов
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    
    ; Вывод сообщения
    mov si, msg
    call print_string
    
    ; Загрузка ядра (сектор 2 и далее)
    mov ah, 0x02        ; функция чтения
    mov al, 4           ; читаем 4 сектора (2КБ для ядра)
    mov ch, 0           ; цилиндр 0
    mov cl, 2           ; сектор 2 (сразу после загрузочного)
    mov dh, 0           ; головка 0
    mov dl, 0           ; дискета A:
    
    ; Куда загружать (сразу после загрузчика)
    mov bx, 0x1000
    
    int 0x13
    jc disk_error
    
    ; Переход к загруженному ядру
    mov si, load_ok_msg
    call print_string
    
    jmp turn_protected

gdt:
    ; Нулевой дескриптор
    db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    ; Сегмент кода: base=0, size=4Gb, P=1, DPL=0, S=1(user),
    ; Type=1(code), Access=00A, G=1, B=32bit
    db 0xff, 0xff, 0x00, 0x00, 0x00, 0x9A, 0xCF, 0x00
    ; Сегмент данных: base=0, size=4Gb, P=1, DPL=0, S=1(user),
    ; Type=0(data), Access=0W0, G=1, B=32bit
    db 0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00
gdt_info: ; Данные о таблице GDT (размер, положение в памяти)
    dw gdt_info - gdt - 1 ; Размер таблицы (2 байта)
    dw gdt, 0 ; 32-битный физический адрес таблицы.

turn_protected:
    ; Отключение прерываний
    cli

    ; Загрузка размера и адреса таблицы дескрипторов
    lgdt [gdt_info] ; Для GNU assembler должно быть "lgdt gdt_info"

     ; Включение адресной линии А20
    in al, 0x92
    or al, 2
    out 0x92, al
 
    ; Установка бита PE регистра CR0 - процессор перейдет в защищенный режим
    mov eax, cr0
    or al, 1
    mov cr0, eax

    jmp 0x8:protected_mode ; "Дальний" переход для загрузки корректной информации в cs (архитектурные особенности не позволяют этого сделать напрямую).

use32
protected_mode:
    ; Загрузка селекторов сегментов для стека и данных в регистры
    mov ax, 0x10 ; Используется дескриптор с номером 2 в GDT
    mov es, ax
    mov ds, ax
    mov ss, ax
    mov esp, 0x90000
     
    ; Передача управления загруженному ядру
    call 0x1000 ; Адрес равен адресу загрузки в случае если ядро скомпилировано в "плоский" код

    jmp $
    
disk_error:
    mov si, error_msg
    call print_string
    jmp $

print_string:
    mov ah, 0x0E
.next_char:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .next_char
.done:
    ret

msg db "Bootloader starting...", 13, 10, 0
error_msg db "Disk error!", 13, 10, 0
load_ok_msg db "Kernel loaded!", 13, 10, 0

; Заполнение до 510 байт
times 510 - ($ - $$) db 0
dw 0xAA55

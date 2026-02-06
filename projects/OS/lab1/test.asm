use16
org 0x7C00

macro print let
{
    mov al, let
    mov ah, 0x0e
    int 0x10
}

start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Отладка
    print 'S'
    print ' '
    
    ; ЗАГРУЗКА ЯДРА С ДИСКА A: (СЕКТОР 1!)
    print 'R'
    print ' '
    
    mov ah, 0x02
    mov al, 1           ; 1 сектор
    mov ch, 0           ; цилиндр 0
    mov cl, 1           ; СЕКТОР 1 (важно!)
    mov dh, 0           ; головка 0
    mov dl, 0x00        ; диск A:
    
    mov ax, 0x1000      ; сегмент 0x1000
    mov es, ax
    xor bx, bx          ; смещение 0
    
    int 0x13
    jc disk_error
    
    print 'O'           ; OK
    print 'K'
    print ' '
    
    jmp turn_protected

disk_error:
    print 'E'
    print ' '
    mov al, ah          ; код ошибки
    call print_hex
    jmp $

print_hex:
    push ax
    shr al, 4
    call .nibble
    pop ax
    and al, 0x0F
    call .nibble
    ret
.nibble:
    cmp al, 10
    jl .digit
    add al, 7
.digit:
    add al, '0'
    mov ah, 0x0e
    int 0x10
    ret

gdt:
    db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    db 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x9A, 0xCF, 0x00
    db 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00
gdt_info:
    dw gdt_info - gdt - 1
    dd gdt

turn_protected:
    print 'P'
    print ' '
    
    cli
    lgdt [gdt_info]
    
    in al, 0x92
    or al, 2
    out 0x92, al
    
    mov eax, cr0
    or al, 1
    mov cr0, eax
    
    jmp 0x8:protected_mode

use32
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x90000
    
    call 0x10000
    jmp $

times 510 - ($ - $$) db 0
db 0x55, 0xAA

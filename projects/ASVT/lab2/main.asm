.org $000
    JMP reset
.org INT0addr
    JMP ext_int0
.org INT1addr
    JMP ext_int1

.equ FREQ1 = 0
.equ FREQ2 = 1
.equ MODE_BUTTON = 2
.equ FREQ_BUTTON = 3
.equ MODE1 = 4
.equ MODE2 = 5
.equ STATE_BIT = 6
.equ ENTER_BUTTON = 7

.def NULL = R16
.def TMP = R17
.def NUM_D = R12
.def VAL1 = R18
.def VAL2 = R19
.def PLUS_Y = R8
.def MINUS_Y = R9
.def STATE = R20
.def FREQ_VAL = R21
.def MODE_VAL = R22
.def TMP_CYCLE = R23

reset: 
    ; Настройка стека
    LDI R20, HIGH(RAMEND)
    OUT SPH, R20
    LDI R20, LOW(RAMEND)
    OUT SPL, R20

    ; Настройка портов

    ; настройка исходных значений
    CLR NULL ; 0x00
    LDI PLUS_Y, 0x55
    CLR NUM_D
    CLR STATE
    CLR FREQ_VAL
    CLR MODE_VAL
    ; настройка портов ввода-вывода
    SER TMP
    OUT DDRA, TMP ; Вывод
    OUT DDRB, TMP ; Вывод
    CLR TMP ; 0x00
    OUT DDRC, TMP ; Ввод
    LDI TMP, 0x73 ; 0xCE
    OUT DDRD, TMP ; 0,1,4,5 - вывод, 2,3,7 - ввод

    ; Настройка прерываний
    LDI TMP, 0x0F
    OUT MCUCR, TMP ; Настройка прерываний int0 и int1 на условие 0/1
    LDI TMP, 0xC0
    OUT GICR, TMP
    OUT GIFR, TMP
    SEI

main_loop:
    IN TMP, PIND
    SBIS TMP, ENTER_BUTTON
    CALL read_y

    CALL update_leds
    CALL delay_freq

    LDI TMP, (1 << STATE_BIT)
    EOR STATE, TMP
    EOR NUM_D, TMP 

    CALL update_output

    RJMP main_loop

update_leds:
    SBRS NUM_D, MODE1 ; Если первый бит установлен прыгаем на проверку второго бита
    RJMP mode2
    SBRS NUM_D, MODE2 ; Если второй бит установлен прыгаем в задержку 15, если не установлен то прыгаем в задержку 5
    RJMP mode1
    RJMP mode3 

mode1:
    LDI R10, 0xFF
    LDI R11, 0x00
    RJMP check_state

mode2:
    LDI R10, 0xAA
    LDI R11, 0x55
    RJMP check_state

mode3:
    MOV R10, PLUS_Y
    CALL calc_neg_y
    MOV R11, MINUS_Y

check_state: 
    CPI STATE, 0
    BREQ out_leds
    
    MOV TMP, R10
    MOV R10, R11
    MOV R11, TMP

out_leds:
    OUT PORTA, R10
    OUT PORTB, R11

    RET

ext_int0:
    MOV TMP_CYCLE, FREQ_VAL
    CALL inc_val
    MOV FREQ_VAL, TMP_CYCLE

    SBRS FREQ_VAL, 0
    CBR NUM_D, FREQ1
    SBRC FREQ_VAL, 0
    SBR NUM_D, FREQ1
    
    SBRS FREQ_VAL, 1
    CBR NUM_D, FREQ2
    SBRC FREQ_VAL, 1
    SBR NUM_D, FREQ2

    RETI


ext_int1:
    MOV TMP_CYCLE, MODE_VAL
    CALL inc_val
    MOV MODE_VAL, TMP_CYCLE

    SBRS MODE_VAL, 0
    CBR NUM_D, MODE1
    SBRC MODE_VAL, 0
    SBR NUM_D, MODE1
    
    SBRS MODE_VAL, 1
    CBR NUM_D, MODE2
    SBRC MODE_VAL, 1
    SBR NUM_D, MODE2

    RETI

inc_val:
    INC TMP_CYCLE        ; увеличить на 1
    CPI TMP_CYCLE, 3     ; если >= 3
    BRLO ok_inc
    LDI TMP_CYCLE, 0     ; сбросить в 0
ok_inc:
    RET


update_output:
    OUT PORTD, NUM_D

calc_neg_y:    
    ; Для прямого кода: инвертируем старший бит (бит знака)
    MOV MINUS_Y, PLUS_Y
    LDI TMP, 0x80 ; Маска для старшего бита
    EOR MINUS_Y, TMP ; Инверсия знакового бита
    
    RET

read_y: ; считывание операнда
    IN TMP, PINC
    CALL delay ; ожидание нажатия комбинации кнопок
    MOV PLUS_Y, TMP
stop_reading: ; обеспечение однократного ввода
    IN TMP, PINC
    CP TMP, NULL ; PINC = 0?
    BRNE stop_reading ; ожидание условия PINC == 0
    RET

delay_freq:
    SBRS NUM_D, FREQ1 ; Если первый бит установлен прыгаем на проверку второго бита
    RJMP delay_1
    SBRS NUM_D, FREQ2 ; Если второй бит установлен прыгаем в задержку 15, если не установлен то прыгаем в задержку 5
    RJMP delay_05
    RJMP delay_15 
delay_1:
    LDI R29, 1
    LDI R30, 1
    LDI R31, 1
    RJMP delay_freq_loop
delay_05:
    LDI R29, 05
    LDI R30, 05
    LDI R31, 05
    RJMP delay_freq_loop
delay_15:
    LDI R29, 15
    LDI R30, 15
    LDI R31, 15
    RJMP delay_freq_loop
delay: ; задержка 120 мс
    LDI R31, 5
    LDI R30, 223
    LDI R29, 188

delay_freq_loop:
    DEC R29
    BRNE delay_freq_loop
    DEC R30
    BRNE delay_freq_loop
    DEC R31
    BRNE delay_freq_loop
    NOP
    NOP
    RET


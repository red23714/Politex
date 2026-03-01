.org $000
    JMP reset
.org INT0Aaddr
    JMP ext_int0
.org INT1addr
    JMP ext_int1

.def NULL = R16
.def TMP = R17
.def MODE1 = R4
.def MODE2 = R5
.def ENTER_NUM = R7
.def FREQ1 = R0
.def FREQ2 = R1
.def VAL1 = R18
.def VAL2 = R19
.def STATE = R6
.def PLUS_Y = R8
.def MINUS_Y = R9

reset: 
    ; Настройка стека
    LDI R20, HIGH(RAMEND)
    OUT SPH, R20
    LDI R20, LOW(RAMEND)
    OUT SPL, R20
    SER TMP

    ; Настройка портов

    ; Настройка прерываний
    OUT MCUCR, TMP ; Настройка прерываний int0 и int1 на условие 0/1
    LDI TMP, 0xC0
    OUT GICR, TMP
    OUT GIFR, TMP
    SEI



mode1:
    LDI R10, 0xFF
    LDI R11, 0x00
    RJMP mode_loop

mode2:
    LDI R10, 0xAA
    LDI R11, 0x55
    RJMP mode_loop

mode3:
    LDI R10, PLUS_Y

    LDI TMP, PLUS_Y
    SBRC TMP, 0 ; Прыгаем в pos если бит 0 очишен
    RJMP pos
    RJMP neg
pos:
    SBR TMP, 0 ; Установка первого бита регистра в единицу
    RJMP main3
neg:
    CBR TMP, 0 ; Установка первого бита решистра в ноль
main3:
    LDI MINUS_Y, LDI
    
    LDI R11, MINUS_Y

mode_loop:
    OUT PORTA, R10
    OUT PORTB, R11

    LDI TMP, R10
    LDI R10, R11
    LDI R11, TMP

    CALL delay_freq

    EOR STATE, STATE

    RJMP mode_loop 

ext_int0:
    
ext_int1:



delay_freq:
    SBRS FREQ1, 0 ; Если первый бит установлен прыгаем на проверку второго бита
    RJMP delay_1
    SBRS FREQ2, 0 ; Если второй бит установлен прыгаем в задержку 15, если не установлен то прыгаем в задержку 5
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


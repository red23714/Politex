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
.equ EEPROM_ADR_FREQ = 0x10
.equ EEPROM_ADR_MODE = 0x11

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
.def EEPROM_VALUE = R24
.def EEPROM_ADR = R25

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
   
    LDI EEPROM_ADR, EEPROM_ADR_FREQ
    CALL EEPROM_read
    MOV FREQ_VAL, EEPROM_VALUE

    LDI EEPROM_ADR, EEPROM_ADR_MODE
    CALL EEPROM_read
    MOV MODE_VAL, EEPROM_VALUE

    ; восстановление частоты
    SBRS FREQ_VAL,0
    CBR NUM_D,(1<<FREQ1)
    SBRC FREQ_VAL,0
    SBR NUM_D,(1<<FREQ1)

    SBRS FREQ_VAL,1
    CBR NUM_D,(1<<FREQ2)
    SBRC FREQ_VAL,1
    SBR NUM_D,(1<<FREQ2)

    ; восстановление режима
    SBRS MODE_VAL,0
    CBR NUM_D,(1<<MODE1)
    SBRC MODE_VAL,0
    SBR NUM_D,(1<<MODE1)

    SBRS MODE_VAL,1
    CBR NUM_D,(1<<MODE2)
    SBRC MODE_VAL,1
    SBR NUM_D,(1<<MODE2)

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

    CALL update_output

main_loop:
    IN TMP, PIND
    SBIS TMP, ENTER_BUTTON
    RJMP input_mode

    CALL update_leds
    CALL delay_freq

    LDI TMP, (1 << STATE_BIT)
    EOR STATE, TMP
    EOR NUM_D, TMP 

    CALL update_output

    RJMP main_loop

input_mode:
    CALL read_y

wait_release:
    IN TMP, PIND
    SBRS TMP, ENTER_BUTTON
    RJMP wait_release

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

ext_int1:
    PUSH TMP
    PUSH TMP_CYCLE

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

    LDI EEPROM_ADR, EEPROM_ADR_FREQ
    MOV EEPROM_VALUE, FREQ_VAL
    CALL EEPROM_write

    POP TMP_CYCLE
    POP TMP

    RETI


ext_int0:
    PUSH TMP
    PUSH TMP_CYCLE

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

    LDI EEPROM_ADR, EEPROM_ADR_MODE
    MOV EEPROM_VALUE, MODE_VAL
    CALL EEPROM_write

    POP TMP_CYCLE
    POP TMP

    RETI

EEPROM_write:
    SBIC EECR, EEWE
    RJMP EEPROM_write

    OUT EEAR, EEPROM_ADR
    OUT EEDR, EEPROM_VALUE

    SBI EECR, EEMWE ; Master Wirte Enable
    SBI EECR, EEWE ; Запуск записи

    RET

EEPROM_read:
    SBIC EECR, EEWE ; проверка на занятость, EECR управляющий регистр EEPROM, EEWE бит проверка на свободность 
    RJMP EEPROM_read

    OUT EEAR, EEPROM_ADR ; устанавливаем значение регистра для чтения адресом которым хотим прочитать значение
    SBI EECR, EERE ; Бит EERE говорит что нужно прочитать по нужному адресу, после чего контроллер автоматически записывает в EEDR
    IN EEPROM_VALUE, EEDR

    RET

inc_val:
    INC TMP_CYCLE        ; увеличить на 1
    CPI TMP_CYCLE, 3     ; если >= 3
    BRLO ok_inc
    LDI TMP_CYCLE, 0     ; сбросить в 0
ok_inc:
    RET


update_output:
    OUT PORTD, NUM_D

    RET

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
    LDI R29, 28
    LDI R30, 186
    LDI R31, 250
    RJMP delay_freq_loop
delay_05:
    LDI R29, 43
    LDI R30, 245
    LDI R31, 250
    RJMP delay_freq_loop
delay_15:
    LDI R29, 29
    LDI R30, 150
    LDI R31, 200
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


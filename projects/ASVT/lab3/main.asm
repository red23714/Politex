.org $000
    JMP reset
.org INT1addr
    JMP ext_int1
.org OVF0addr
    JMP ISR_T0

.dseg
    DELAY_CNT:    .byte 2
    TIMEOUT_CNT:  .byte 2
    DISP_POS:     .byte 1
    BLINK_CNT:    .byte 1
    BLINK_STATE:  .byte 1
.cseg

SEG_TABLE:
.db 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F

.equ EEPROM_ADR_PIN1 = 0x10
.equ EEPROM_ADR_PIN2 = 0x11

.def NULL                = R16
.def TMP                 = R17
.def TMP2                = R18
.def CURRENT_CELL        = R19
.def RIGHT_PIN1          = R20
.def RIGHT_PIN2          = R21
.def EEPROM_VALUE        = R22
.def EEPROM_ADR          = R23
.def CURRENT_PIN1        = R24
.def CURRENT_PIN2        = R25
.def ATTEMPT_COUNT       = R26
.def CURRENT_NUMBER      = R27
.def CURRENT_READ_NUMBER = R28
.def START_NEW           = R29

reset:
    LDI TMP, HIGH(RAMEND)
    OUT SPH, TMP
    LDI TMP, LOW(RAMEND)
    OUT SPL, TMP

    CLR NULL

    LDI EEPROM_ADR, EEPROM_ADR_PIN1
    CALL EEPROM_read
    MOV RIGHT_PIN1, EEPROM_VALUE

    LDI EEPROM_ADR, EEPROM_ADR_PIN2
    CALL EEPROM_read
    MOV RIGHT_PIN2, EEPROM_VALUE

    CLR TMP
    OUT DDRB, TMP           ; PORTB вход (кнопки 0-7)
    LDI TMP, 0xCF
    OUT DDRA, TMP           ; PA0-PA3 выход (разряды дисплея),
                            ; PA4-PA5 вход (кнопки 8-9),
                            ; PA6-PA7 выход (светодиоды)
    LDI TMP, 0xDF
    OUT DDRD, TMP           ; PD5 выход, остальное вход (PD3=INT1)
    SER TMP
    OUT DDRC, TMP           ; PORTC выход (сегменты дисплея)

    ; INT0 и INT1 по фронту
    LDI TMP, 0x0F
    OUT MCUCR, TMP
    LDI TMP, 0xC0
    OUT GICR, TMP
    OUT GIFR, TMP

    ; таймер T0: прескалер 64, перезагрузка = 1мс
    LDI TMP, (1<<CS01)|(1<<CS00)
    OUT TCCR0, TMP
    LDI TMP, 131
    OUT TCNT0, TMP
    LDI TMP, (1<<TOIE0)
    OUT TIMSK, TMP

    ; инициализация SRAM
    CLR TMP
    STS DISP_POS,     TMP
    STS DELAY_CNT,    TMP
    STS DELAY_CNT+1,  TMP
    STS TIMEOUT_CNT,  TMP
    STS TIMEOUT_CNT+1,TMP
    STS BLINK_CNT,    TMP
    STS BLINK_STATE,  TMP

    ; ATTEMPT_COUNT сбрасываем ТОЛЬКО здесь (при включении питания)
    CLR ATTEMPT_COUNT

    SEI

; soft_reset — возврат к началу ввода БЕЗ сброса ATTEMPT_COUNT
soft_reset:
    CLR TMP
    CLR TMP2
    CLR CURRENT_CELL
    CLR CURRENT_NUMBER
    CLR CURRENT_READ_NUMBER
    CLR START_NEW

    ; дисплей пуст
    LDI TMP, 0xFF
    MOV CURRENT_PIN1, TMP
    MOV CURRENT_PIN2, TMP

    ; сброс таймаута и мигания
    CLR TMP
    STS TIMEOUT_CNT,   TMP
    STS TIMEOUT_CNT+1, TMP
    STS BLINK_CNT,     TMP
    STS BLINK_STATE,   TMP

    ; гасим светодиоды и дисплей
    IN  TMP, PORTA
    ANDI TMP, 0x3F          ; PA6=0, PA7=0 (светодиоды выкл)
    OUT PORTA, TMP

main_loop:
    RCALL read_number
    CPI CURRENT_CELL, 4
    BREQ check_correct
    RJMP main_loop

read_number:
    ; если уже введена хоть одна цифра — проверяем таймаут
    TST  CURRENT_CELL
    BREQ RN_WAIT
    LDS  TMP,  TIMEOUT_CNT
    LDS  TMP2, TIMEOUT_CNT+1
    OR   TMP,  TMP2
    BREQ soft_reset         ; таймаут истёк → сброс

RN_WAIT:
    IN   TMP,  PINB
    IN   TMP2, PINA

    ; проверить PORTB (кнопки 0-7)
    CPSE TMP, NULL
    RJMP read_b

    ; проверить PA4 и PA5 (кнопки 8 и 9)
    MOV  R14, TMP2
    ANDI R14, 0x30
    TST  R14
    BRNE read_a

    ; ничего не нажато — перепроверить таймаут
    TST  CURRENT_CELL
    BREQ RN_WAIT
    LDS  TMP,  TIMEOUT_CNT
    LDS  TMP2, TIMEOUT_CNT+1
    OR   TMP,  TMP2
    BREQ soft_reset
    RJMP RN_WAIT

read_b:
    MOV  CURRENT_READ_NUMBER, TMP
stop_reading_b:
    IN   TMP, PINB
    CP   TMP, NULL
    BRNE stop_reading_b
    CLR  CURRENT_NUMBER
    RJMP get_number_b

read_a:
    ; PA4=кнопка 8, PA5=кнопка 9
    CLR  CURRENT_READ_NUMBER
    SBRC TMP2, 4
    LDI  CURRENT_READ_NUMBER, 8
    SBRC TMP2, 5
    LDI  CURRENT_READ_NUMBER, 9
stop_reading_a:
    IN   TMP2, PINA
    ANDI TMP2, 0x30
    TST  TMP2
    BRNE stop_reading_a
    MOV  CURRENT_NUMBER, CURRENT_READ_NUMBER
    RJMP apply_digit

inc_current_number:
    LSR  CURRENT_READ_NUMBER
    INC  CURRENT_NUMBER

get_number_b:
    SBRS CURRENT_READ_NUMBER, 0
    RJMP inc_current_number
    ; CURRENT_NUMBER теперь содержит номер нажатой кнопки (0-7)

apply_digit:
    ; перезапустить таймаут 7000 мс
    CLI
    LDI  TMP, LOW(7000)
    STS  TIMEOUT_CNT,   TMP
    LDI  TMP, HIGH(7000)
    STS  TIMEOUT_CNT+1, TMP
    SEI

    ; записать цифру в нужный nibble CURRENT_PIN1 или CURRENT_PIN2
    MOV  TMP, CURRENT_NUMBER
    ANDI TMP, 0x0F

    SBRS CURRENT_CELL, 0    ; бит 0 = 1 → нечётная ячейка → hi nibble
    RJMP AD_LO

    SWAP TMP
    ANDI TMP, 0xF0
    CPI  CURRENT_CELL, 3
    BRSH AD_PIN2_HI
    ANDI CURRENT_PIN1, 0x0F
    OR   CURRENT_PIN1, TMP
    RJMP AD_DONE
AD_PIN2_HI:
    ANDI CURRENT_PIN2, 0x0F
    OR   CURRENT_PIN2, TMP
    RJMP AD_DONE

AD_LO:
    CPI  CURRENT_CELL, 2
    BRSH AD_PIN2_LO
    ANDI CURRENT_PIN1, 0xF0
    OR   CURRENT_PIN1, TMP
    RJMP AD_DONE
AD_PIN2_LO:
    ANDI CURRENT_PIN2, 0xF0
    OR   CURRENT_PIN2, TMP

AD_DONE:
    INC  CURRENT_CELL

    ; после 4-й цифры таймаут не нужен
    CPI  CURRENT_CELL, 4
    BRLO AD_RET
    CLR  TMP
    STS  TIMEOUT_CNT,   TMP
    STS  TIMEOUT_CNT+1, TMP
AD_RET:
    RET

check_correct:
    CP  CURRENT_PIN1, RIGHT_PIN1
    BRNE incorrect
    CP  CURRENT_PIN2, RIGHT_PIN2
    BRNE incorrect
    RJMP correct

incorrect:
    INC  ATTEMPT_COUNT
    CPI  ATTEMPT_COUNT, 3
    BRSH lose                   ; >= 3 неверных подряд

    ; зажечь PA6 на 20 секунд
    IN   TMP, PORTA
    ORI  TMP, (1<<PA6)
    ANDI TMP, ~(1<<PA7)         ; PA7 выкл на всякий случай
    OUT  PORTA, TMP
    LDI  R15, HIGH(20000)
    LDI  R14, LOW(20000)
    CALL DELAY_MS
    IN   TMP, PORTA
    ANDI TMP, ~(1<<PA6)         ; гасим PA6
    OUT  PORTA, TMP
    RJMP soft_reset

correct:
    ; сбросить счётчик попыток — успешный вход
    CLR  ATTEMPT_COUNT

    ; зажечь PA7
    IN   TMP, PORTA
    ORI  TMP, (1<<PA7)
    OUT  PORTA, TMP

    ; ждём INT1 (PD3)
wait_new:
    CPI  START_NEW, 1
    BRNE wait_new

    ; очистить дисплей и погасить PA7
    LDI  TMP, 0xFF
    MOV  CURRENT_PIN1, TMP
    MOV  CURRENT_PIN2, TMP
    CLR  START_NEW
    IN   TMP, PORTA
    ANDI TMP, ~(1<<PA7)
    OUT  PORTA, TMP
    RJMP soft_reset

lose:
    ; PA6 + PA7 горят, программа останавливается
    IN   TMP, PORTA
    ORI  TMP, (1<<PA7)|(1<<PA6)
    OUT  PORTA, TMP
lose_loop:
    RJMP lose_loop

ext_int1:
    PUSH TMP
    IN   TMP, SREG
    PUSH TMP
    LDI  START_NEW, 1
    POP  TMP
    OUT  SREG, TMP
    POP  TMP
    RETI

EEPROM_read:
    SBIC EECR, EEWE
    RJMP EEPROM_read
    OUT  EEARL, EEPROM_ADR
    CLR  TMP
    OUT  EEARH, TMP
    SBI  EECR, EERE
    IN   EEPROM_VALUE, EEDR
    RET

ISR_T0:
    PUSH TMP
    IN   TMP, SREG
    PUSH TMP
    PUSH TMP2

    LDI  TMP, 131
    OUT  TCNT0, TMP

    ; декремент DELAY_CNT
    LDS  TMP,  DELAY_CNT
    LDS  TMP2, DELAY_CNT+1
    OR   TMP2, TMP
    BREQ ISR_SKIP_DELAY
    LDS  TMP, DELAY_CNT
    SUBI TMP, 1
    STS  DELAY_CNT, TMP
    LDS  TMP, DELAY_CNT+1
    SBCI TMP, 0
    STS  DELAY_CNT+1, TMP
ISR_SKIP_DELAY:

    ; декремент TIMEOUT_CNT
    LDS  TMP,  TIMEOUT_CNT
    LDS  TMP2, TIMEOUT_CNT+1
    OR   TMP2, TMP
    BREQ ISR_SKIP_TIMEOUT
    LDS  TMP, TIMEOUT_CNT
    SUBI TMP, 1
    STS  TIMEOUT_CNT, TMP
    LDS  TMP, TIMEOUT_CNT+1
    SBCI TMP, 0
    STS  TIMEOUT_CNT+1, TMP
ISR_SKIP_TIMEOUT:

    ; мигание 2 Гц (переключение каждые 250 мс)
    LDS  TMP, BLINK_CNT
    INC  TMP
    STS  BLINK_CNT, TMP
    CPI  TMP, 250
    BRLO ISR_SKIP_BLINK
    CLR  TMP
    STS  BLINK_CNT, TMP
    LDS  TMP, BLINK_STATE
    COM  TMP
    ANDI TMP, 0x01
    STS  BLINK_STATE, TMP
ISR_SKIP_BLINK:

    RCALL DISPLAY_REFRESH

    POP  TMP2
    POP  TMP
    OUT  SREG, TMP
    POP  TMP
    RETI

DELAY_MS:
    CLI
    STS  DELAY_CNT,   R14
    STS  DELAY_CNT+1, R15
    SEI
DELAY_WAIT:
    LDS  TMP,  DELAY_CNT
    LDS  TMP2, DELAY_CNT+1
    OR   TMP,  TMP2
    BRNE DELAY_WAIT
    RET

DISPLAY_REFRESH:
    PUSH ZL
    PUSH ZH
    PUSH R30

    ; выключить все разряды PA0-PA3
    IN   TMP, PORTA
    ANDI TMP, 0xF0
    OUT  PORTA, TMP

    LDS  TMP2, DISP_POS     ; текущий разряд 0..3

    CPI  TMP2, 0
    BREQ DR_DIG0
    CPI  TMP2, 1
    BREQ DR_DIG1
    CPI  TMP2, 2
    BREQ DR_DIG2
    ; разряд 3 = hi nibble PIN2
    MOV  R30, CURRENT_PIN2
    SWAP R30
    ANDI R30, 0x0F
    RJMP DR_GOT
DR_DIG2:
    MOV  R30, CURRENT_PIN2
    ANDI R30, 0x0F
    RJMP DR_GOT
DR_DIG1:
    MOV  R30, CURRENT_PIN1
    SWAP R30
    ANDI R30, 0x0F
    RJMP DR_GOT
DR_DIG0:
    MOV  R30, CURRENT_PIN1
    ANDI R30, 0x0F

DR_GOT:
    ; мигание текущей ячейки
    CP   TMP2, CURRENT_CELL
    BRNE DR_NO_BLINK
    LDS  TMP, BLINK_STATE
    CPI  TMP, 1
    BREQ DR_BLANK
DR_NO_BLINK:
    ; пустая ячейка
    CPI  R30, 0x0F
    BREQ DR_BLANK

    LDI  ZL, LOW(SEG_TABLE*2)
    LDI  ZH, HIGH(SEG_TABLE*2)
    ADD  ZL, R30
    BRCC DR_NO_CARRY
    INC  ZH
DR_NO_CARRY:
    LPM  TMP, Z
    OUT  PORTC, TMP
    RJMP DR_ENABLE

DR_BLANK:
    CLR  TMP
    OUT  PORTC, TMP

DR_ENABLE:
    ; включить разряд TMP2 через PA0-PA3
    LDI  TMP, 0x01
    MOV  R30, TMP2
    TST  R30
    BREQ DR_OUT
DR_SHIFT:
    LSL  TMP
    DEC  R30
    BRNE DR_SHIFT
DR_OUT:
    IN   TMP2, PORTA
    ANDI TMP2, 0xF0
    OR   TMP2, TMP
    OUT  PORTA, TMP2

    LDS  TMP, DISP_POS
    INC  TMP
    ANDI TMP, 0x03
    STS  DISP_POS, TMP

    POP  R30
    POP  ZH
    POP  ZL
    RET

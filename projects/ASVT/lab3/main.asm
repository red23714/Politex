.include "m32def.inc"

.def  rA = r16                 ; рабочий регистр A
.def  rB = r17                 ; рабочий регистр B
.def  rC = r18                 ; рабочий регистр C

.dseg
pin_cifry:      .byte 4        ; 4 цифры пин-кода
tekush_poz:     .byte 1        ; на какой цифре сейчас стоим (0..3)
mig_schetchik:  .byte 1        ; считаем тики до следующего моргания
mig_sostoyanie: .byte 1        ; 0 = курсор виден, 1 = курсор скрыт
taymaut_ml:     .byte 1        ; младший байт счётчика таймаута
taymaut_st:     .byte 1        ; старший байт счётчика таймаута
taymaut_vkl:    .byte 1        ; 1 = таймаут идёт, 0 = не идёт
multipleks:     .byte 1        ; какой разряд дисплея сейчас светится
prev_portB:     .byte 1        ; состояние порта B на прошлом шаге
prev_portA:     .byte 1        ; состояние порта A на прошлом шаге
prev_portD:     .byte 1        ; состояние порта D на прошлом шаге

.cseg

.org 0x0000
    JMP reset
.org INT1addr
    JMP ext_int1
.org OC2addr
    RJMP  prervanie_taimer2    ; прерывание таймера 2 — мигание и таймаут
.org OC0addr
    RJMP  prervanie_taimer0    ; прерывание таймера 0 — обновление дисплея

; сбрасываем счётчик мигания и гасим курсор
sbros_miganiya:
    CLR   rA
    STS   mig_schetchik,  rA   ; счётчик в ноль
    STS   mig_sostoyanie, rA   ; курсор показываем
    RET

; останавливаем таймаут и сбрасываем его счётчик
sbros_taymaut:
    CLR   rA
    STS   taymaut_ml,  rA      ; младший байт в ноль
    STS   taymaut_st,  rA      ; старший байт в ноль
    STS   taymaut_vkl, rA      ; выключаем таймаут
    RET



.equ EEPROM_ADR_PIN1 = 0x10
.equ EEPROM_ADR_PIN2 = 0x11

.def NULL = R28
.def TMP = R29
.def TMP2 = R30
.def CURRENT_CELL = R19
.def RIGHT_PIN1 = R20
.def RIGHT_PIN2 = R21
.def EEPROM_VALUE = R22
.def EEPROM_ADR = R23
.def CURRENT_PIN1 = R24
.def CURRENT_PIN2 = R25
.def ATTEMPT_COUNT = R26
.def CURRENT_NUMBER = R27
.def CURRENT_READ_NUMBER = R28
.def START_NEW = R29

reset: 
    LDI TMP, HIGH(RAMEND)
    OUT SPH, TMP
    LDI TMP, LOW(RAMEND)
    OUT SPL, TMP

    CLR NULL ; 0x00
    CLR ATTEMPT_COUNT
   
    LDI EEPROM_ADR, EEPROM_ADR_PIN1
    CALL EEPROM_read
    MOV RIGHT_PIN1, EEPROM_VALUE

    LDI EEPROM_ADR, EEPROM_ADR_PIN2
    CALL EEPROM_read
    MOV RIGHT_PIN2, EEPROM_VALUE

    CLR TMP ; 0x00
    OUT DDRB, TMP ; ????
    LDI TMP, 0xC0
    OUT DDRA, TMP 
    LDI TMP, 0xF3
    OUT DDRD, TMP
    
    CLR TMP
    STS   tekush_poz,     TMP
    STS   mig_schetchik,  TMP
    STS   mig_sostoyanie, TMP
    STS   taymaut_ml,     TMP
    STS   taymaut_st,     TMP
    STS   taymaut_vkl,    TMP
    STS   multipleks,     TMP

    LDI   TMP, 0x0B
    OUT   TCCR0, TMP            ; таймер 0: режим CTC, делитель /64
    LDI   TMP, 124
    OUT   OCR0, TMP             ; TOP=124, при 8МГц даёт прерывание каждую мс

    LDI   TMP, 0x4F
    OUT   TCCR2, TMP            ; таймер 2: режим CTC, делитель /1024
    LDI   TMP, 77
    OUT   OCR2, TMP             ; TOP=77, даёт ~100 прерываний в секунду

    LDI   TMP, (1<<OCIE2)|(1<<OCIE0)
    OUT   TIMSK, TMP            ; разрешаем прерывания от обоих таймеров
 

    LDI TMP, 0x0F
    OUT MCUCR, TMP ; ????????? ?????????? int0 ? int1 ?? ??????? 0/1
    LDI TMP, 0xC0
    OUT GICR, TMP
    OUT GIFR, TMP
    SEI

soft_reset:
    CLR TMP
    CLR TMP2
    CLR CURRENT_CELL
    CLR CURRENT_PIN1
    CLR CURRENT_PIN2
    CLR CURRENT_NUMBER
    CLR CURRENT_READ_NUMBER
    CLR START_NEW
    OUT PORTA, TMP

main_loop:
    RCALL read_number

    CPI CURRENT_CELL, 4
    BREQ check_correct

    RJMP main_loop

read_number:
    IN TMP, PINB
    IN TMP2, PINA

    CPSE TMP, NULL 
    RJMP read_b

    CPSE TMP2, NULL
    RJMP read_a

    RJMP read_number

read_b:
    MOV CURRENT_READ_NUMBER, TMP
stop_reading_b:
    IN TMP, PINB
    CP TMP, NULL
    BRNE stop_reading_b
    RJMP get_number_b

read_a:
    SBRC TMP2, 4
    LDI CURRENT_READ_NUMBER, 0x08
    SBRC TMP2, 5
    LDI CURRENT_READ_NUMBER, 0x09
stop_reading_a:
    IN TMP2, PINA
    CP TMP2, NULL
    BRNE stop_reading_a
    RJMP get_number_a

inc_current_number:
    LSR CURRENT_READ_NUMBER
    INC CURRENT_NUMBER

get_number_b:
    SBRS CURRENT_READ_NUMBER, 0
    RJMP inc_current_number
get_number_a:
    SBRC CURRENT_CELL, 0
    RJMP set_number
    LSL CURRENT_NUMBER
    LSL CURRENT_NUMBER
    LSL CURRENT_NUMBER
    LSL CURRENT_NUMBER

set_number: 
    CPI CURRENT_CELL, 2
    
    INC CURRENT_CELL

    BRPL set_34
    ADD CURRENT_PIN1, CURRENT_NUMBER
    RET
set_34: 
    ADD CURRENT_PIN2, CURRENT_NUMBER
    RET

; записываем цифру rC на текущую позицию пин-кода
vvod_cifry:
    LDS   rA, tekush_poz       ; берём текущую позицию
    LDI   ZL, LOW(pin_cifry)
    LDI   ZH, HIGH(pin_cifry)
    ADD   ZL, rA               ; смещаем указатель Z на нужный элемент
    BRCC  vc_ok
    INC   ZH                   ; был перенос — поправляем старший байт
vc_ok:
    ST    Z, rC                ; сохраняем цифру

    CLR   rA
    STS   taymaut_ml, rA       ; сбрасываем счётчик таймаута
    STS   taymaut_st, rA       ; пользователь только что нажал кнопку

    LDS   rA, tekush_poz
    CPI   rA, 3                ; это была 4-я (последняя) цифра?
    BRNE  vc_ne_poslednyaya
    RCALL sohranit_pin         ; да — сохраняем пин в EEPROM
    RJMP  zavershenie          ; и завершаем ввод

vc_ne_poslednyaya:
    INC   rA
    STS   tekush_poz, rA       ; переходим к следующей цифре
    RCALL sbros_miganiya       ; сбрасываем мигание для нового разряда
    LDI   rA, 1
    STS   taymaut_vkl, rA      ; запускаем таймаут — ждём следующую цифру
    RET



check_correct:
    CP CURRENT_PIN1, RIGHT_PIN1
    BRNE incorrect
    CP CURRENT_PIN2, RIGHT_PIN2
    BRNE incorrect

    RJMP correct

incorrect:
    INC ATTEMPT_COUNT
    CPI ATTEMPT_COUNT, 0x03
    BRSH lose

    LDI  TMP, (1<<PA6)
    OUT PORTA, TMP
    RJMP soft_reset

correct:
    LDI TMP, (1 << PA7)
    OUT PORTA, TMP
wait_new:
    CPI START_NEW, 1
    BRNE wait_new
    RJMP soft_reset

ext_int1:
    PUSH TMP
    IN TMP, SREG
    
    LDI START_NEW, 1

    OUT SREG, TMP
    POP TMP
    RETI

EEPROM_read:
    SBIC EECR, EEWE ; ???????? ?? ?????????, EECR ??????????? ??????? EEPROM, EEWE ??? ???????? ?? ??????????? 
    RJMP EEPROM_read

    OUT EEARL, EEPROM_ADR
    CLR TMP
    OUT EEARH, TMP ; ????????????? ???????? ???????? ??? ?????? ??????? ??????? ????? ????????? ????????
    SBI EECR, EERE ; ??? EERE ??????? ??? ????? ????????? ?? ??????? ??????, ????? ???? ?????????? ????????????? ?????????? ? EEDR
    IN EEPROM_VALUE, EEDR

    RET

lose:
    LDI  TMP, (1 << PA7)
    ORI TMP, (1 << PA6)

    OUT PORTA, TMP
    RJMP lose

.include "m32def.inc"

.equ EEPROM_ADR_PIN1 = 0x10
.equ EEPROM_ADR_PIN2 = 0x11

.def FLAG_TIMER_20 = R13
.def FLAG_TIMER_7 = R14
.def START_NEW = R15
.def NULL = R16
.def TMP = R17
.def TMP2 = R18
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
.def COUNTER = R29
.def SECONDS = R30

.org $000
    JMP reset
.org INT1addr
    JMP ext_int1
.org OVF0addr
    RJMP TIMER0_OVF


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

    ; Предделитель /1024 — исправлено
    ldi TMP, (1<<CS02)|(1<<CS00)
    out TCCR0, TMP

    CLR COUNTER

    IN TMP, TIMSK
    ORI TMP, (1<<TOIE0)
    OUT TIMSK, TMP

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
    CALL delay

    CP TMP, NULL 
    BREQ read_number

    MOV CURRENT_READ_NUMBER, TMP
stop_reading_b:
    IN TMP, PINB
    CP TMP, NULL
    BRNE stop_reading_b
    RJMP get_number_b

read_a:
    CALL delay

    CP TMP2, NULL 
    BREQ read_number

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

    CLR FLAG_TIMER_20
    LDI TMP, 0
    OUT TCNT0, TMP
    CLR SECONDS

incorrect_loop:
    LDI  TMP, (1<<PA6)
    OUT PORTA, TMP
    
	MOV TMP, FLAG_TIMER_20
    CPI TMP, 1
    BRNE incorrect_loop

    CLR FLAG_TIMER_20

    RJMP soft_reset

correct:
    LDI TMP, (1 << PA7)
    OUT PORTA, TMP
wait_new:
	MOV TMP, START_NEW
    CPI TMP, 1
    BRNE wait_new
    RJMP soft_reset

ext_int1:
    PUSH TMP
    IN TMP, SREG
    
    INC START_NEW

    OUT SREG, TMP
    POP TMP
    RETI

TIMER0_OVF:
    INC COUNTER
    CPI COUNTER, 30
    BRNE OVF_EXIT
    
    INC SECONDS

	OUT PORTC, SECONDS

    CPI SECONDS, 7
    BREQ set_flag_7

    CPI SECONDS, 20
    BREQ set_flag_20

    CLR COUNTER 

    RJMP OVF_EXIT

set_flag_7:
    INC FLAG_TIMER_7
    RJMP OVF_EXIT

set_flag_20:
    INC FLAG_TIMER_20
    RJMP OVF_EXIT

OVF_EXIT:
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

delay: ; ???????? 120 ??
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

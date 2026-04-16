.include "m32def.inc"
.def TMP = R16
.def COUNTER = R17
.def IS_SET = R18

.org 0x00
    RJMP reset
.org OVF0addr
    RJMP TIMER0_OVF

reset: 
    LDI TMP, HIGH(RAMEND)
    OUT SPH, TMP
    LDI TMP, LOW(RAMEND)
    OUT SPL, TMP

    SER TMP
    OUT DDRC, TMP
    LDI TMP, 0b11111111
    OUT DDRA, TMP

    ; Предделитель /1024 — исправлено
    ldi TMP, (1<<CS02)|(1<<CS00)
    out TCCR0, TMP

    CLR COUNTER
    LDI IS_SET, 0x1

    IN TMP, TIMSK
    ORI TMP, (1<<TOIE0)
    OUT TIMSK, TMP

    SEI

main_loop:
    LDI TMP, 0b00000001
    OUT PORTA, TMP
    CPI IS_SET, 0
    BRNE on_disp
    
    LDI TMP, 0b00000000
    OUT PORTC, TMP
    RJMP main_loop

on_disp:
    LDI TMP, 0b11111111
    OUT PORTC, TMP
    RJMP main_loop

TIMER0_OVF:
    INC COUNTER
    CPI COUNTER, 61
    BRNE OVF_EXIT
    
    CLR COUNTER
    
    ; Исправленное переключение флага
    CPI IS_SET, 0x1
    BRNE set_is_set
    LDI IS_SET, 0x0     ; был 1 — стал 0
    RJMP OVF_EXIT
set_is_set:
    LDI IS_SET, 0x1     ; был 0 — стал 1
OVF_EXIT:
    RETI

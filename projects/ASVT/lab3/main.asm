.include "m32def.inc"

.equ EEPROM_ADR_PIN1 = 0x10
.equ EEPROM_ADR_PIN2 = 0x11

.def ATTEMPT_COUNT = R12
.def FLAG_TIMER_20 = R13
.def FLAG_TIMER_7 = R14
.def START_NEW = R15
.def NULL = R16
.def TMP = R17
.def TMP2 = R18
.def CURRENT_CELL = R19
.def RIGHT_PIN1 = R20
.def RIGHT_PIN2 = R21
.def TIMER_TMP = R22
.def DISP_CELL = R23
.def CURRENT_PIN1 = R24
.def CURRENT_PIN2 = R25
.def CURRENT_NUMBER = R26
.def CURRENT_READ_NUMBER = R27
.def COUNTER = R28
.def SECONDS_7 = R29
.def SECONDS_20 = R30
.def ADC_TMP = R31

.org $000
    JMP reset
.org INT0addr
    JMP ext_int0
.org INT1addr
    JMP ext_int1
.org OVF1addr
    RJMP TIMER1_OVF
.org OC0addr
  RJMP TIMER0_COMP
.org ADCCaddr
    RJMP ADC_ISR


SEG_TABLE:
    .db 0b00111111, 0b00000110   ; 0, 1
    .db 0b01011011, 0b01001111   ; 2, 3
    .db 0b01100110, 0b01101101   ; 4, 5
    .db 0b01111101, 0b00000111   ; 6, 7
    .db 0b01111111, 0b01101111   ; 8, 9
    .db 0b01110111, 0b01111100   ; A, b
    .db 0b00111001, 0b01011110   ; C, d
    .db 0b01111001, 0b01110001   ; E, F

reset: 
    LDI TMP, HIGH(RAMEND)
    OUT SPH, TMP
    LDI TMP, LOW(RAMEND)
    OUT SPL, TMP

    CLR NULL ; 0x00
    CLR ATTEMPT_COUNT
  
    LDI TMP, EEPROM_ADR_PIN1
    CALL EEPROM_read
    MOV RIGHT_PIN1, TMP2

    LDI TMP, EEPROM_ADR_PIN2
    CALL EEPROM_read
    MOV RIGHT_PIN2, TMP2

    CLR TMP ; 0x00
    OUT DDRB, TMP 
    LDI TMP, 0xCF
    OUT DDRA, TMP 
    LDI TMP, 0xF2
    OUT DDRD, TMP
  SER TMP
  OUT DDRC, TMP

    ; ???????????? /1024
    LDI TMP, (1<<CS11)|(1<<CS10)
    OUT TCCR1B, TMP

    ;IN TMP, TIMSK
    LDI TMP, (1<<TOIE1)|(1<<OCIE0)
    OUT TIMSK, TMP

  LDI TMP, (1<<WGM01)|(1<<CS01)|(1<<CS00) ; CTC, /64
  OUT TCCR0, TMP

  LDI TMP, 124
  OUT OCR0, TMP

  LDI  TMP, 0x65
    OUT  ADMUX, TMP

    LDI  TMP, 0x8E
    OUT  ADCSRA, TMP

    ; ????? ??????? ??????????????
    SBI  ADCSRA, ADSC
 

    LDI TMP, 0x0F
    OUT MCUCR, TMP 
    LDI TMP, 0xC0
    OUT GICR, TMP
    OUT GIFR, TMP
    SEI

soft_reset:
    CLR FLAG_TIMER_20
    CLR FLAG_TIMER_7
    CLR START_NEW
    CLR SECONDS_7
    CLR SECONDS_20
    CLR COUNTER
    CLR TMP
    CLR TMP2
    CLR CURRENT_CELL
    CLR CURRENT_PIN1
    CLR CURRENT_PIN2
    CLR CURRENT_NUMBER
    CLR CURRENT_READ_NUMBER
    CLR START_NEW
    OUT PORTA, TMP

  IN   TMP, PORTA
    ANDI TMP, 0b11000000    ; PA0-PA3 = LOW, PA6/PA7 ?? ???????
    OUT  PORTA, TMP
    OUT  PORTC, NULL
    RJMP read_number

check_correct:
    CP CURRENT_PIN1, RIGHT_PIN1
    BRNE incorrect
    CP CURRENT_PIN2, RIGHT_PIN2
    BRNE incorrect

    RJMP correct

incorrect:
    INC ATTEMPT_COUNT
    MOV TMP, ATTEMPT_COUNT
    CPI TMP, 0x03
    BRSH lose

    CLR FLAG_TIMER_20
    CLR TMP
  OUT TCNT1H, TMP
  OUT TCNT1L, TMP
    CLR SECONDS_20
  CLR COUNTER

  IN TMP, PORTA
    ORI  TMP, (1<<PA6)
    OUT PORTA, TMP

incorrect_loop:
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

lose:
    LDI  TMP, (1 << PA7)
    ORI TMP, (1 << PA6)

    OUT PORTA, TMP
    RJMP lose

check_7sec:
    MOV TMP, FLAG_TIMER_7
    CPI TMP, 1
    BREQ rjmp_soft_reset

    RJMP read_number

rjmp_soft_reset:
  RJMP soft_reset

read_number:
  CPI CURRENT_CELL, 4
    BREQ check_correct

    IN TMP, PINB
    IN TMP2, PINA
  ANDI TMP2, 0b00010000

    CPSE TMP, NULL 
    RJMP read_b

    CPSE TMP2, NULL
    RJMP read_a

    RJMP check_7sec

read_b:
    NOP
    NOP
    NOP
    NOP

    IN TMP, PINB
    CP TMP, NULL 
    BREQ read_number
    
    CLR SECONDS_7
    CLR COUNTER

    MOV CURRENT_READ_NUMBER, TMP
stop_reading_b:
    IN TMP, PINB
    CP TMP, NULL
    BRNE stop_reading_b
    CLR CURRENT_NUMBER
    RJMP get_number_b

read_a:
    NOP
    NOP
    NOP
    NOP

    IN TMP2, PINA
    ANDI TMP2, 0b00010000
    CP TMP2, NULL 
    BREQ read_number

    CLR SECONDS_7
    CLR COUNTER
    SBRC TMP2, 4
    LDI CURRENT_NUMBER, 0x08
stop_reading_a:
    IN TMP2, PINA
    ANDI TMP2, 0b00010000
    CP TMP2, NULL
    BRNE stop_reading_a
    RJMP set_number

inc_current_number:
    LSR CURRENT_READ_NUMBER
    INC CURRENT_NUMBER

get_number_b:
    OUT PORTC, CURRENT_READ_NUMBER
    SBRS CURRENT_READ_NUMBER, 0
    RJMP inc_current_number
set_number:
    INC CURRENT_CELL
    CPI CURRENT_CELL, 3
    BRPL set_34
  
    CPI CURRENT_CELL, 2
    BREQ shift_number

write_12:
    OR CURRENT_PIN1, CURRENT_NUMBER
    RJMP read_number
set_34: 
    CPI CURRENT_CELL, 4
    BREQ shift_number

write_34:
    OR CURRENT_PIN2, CURRENT_NUMBER
    RJMP read_number

shift_number:
    LSL CURRENT_NUMBER
    LSL CURRENT_NUMBER
    LSL CURRENT_NUMBER
    LSL CURRENT_NUMBER

  CPI CURRENT_CELL, 3
    BRPL write_34

  RJMP write_12

update_current_cell:
  CLR CURRENT_CELL
  RJMP return_to_ext0

ext_int0:
  PUSH ADC_TMP
    PUSH ADC_TMP
    IN ADC_TMP, SREG
    
    INC CURRENT_CELL
  CPI CURRENT_CELL, 4
  BREQ update_current_cell

return_to_ext0:

  POP ADC_TMP
    OUT SREG, ADC_TMP
    POP ADC_TMP

    RETI

ext_int1:
    PUSH TMP
    IN TMP, SREG
    
    INC START_NEW

    OUT SREG, TMP
    POP TMP
    RETI

TIMER1_OVF:
    INC COUNTER
    CPI COUNTER, 2
    BRNE OVF_EXIT

    CLR COUNTER 

    INC SECONDS_7
    INC SECONDS_20

    CPI SECONDS_7, 7
    BREQ set_flag_7

check_20:
    CPI SECONDS_20, 20
    BREQ set_flag_20

    RJMP OVF_EXIT

set_flag_7:
  CPI CURRENT_CELL, 0
    BRNE cur_cel_0
    CLR SECONDS_7

  RJMP check_20

cur_cel_0:
  INC FLAG_TIMER_7
    RJMP OVF_EXIT

set_flag_20:
    INC FLAG_TIMER_20
    CLR SECONDS_20

OVF_EXIT:
    RETI

TIMER0_COMP:
  PUSH  TIMER_TMP
    IN    TIMER_TMP, SREG             ; ????????? ?????
    PUSH  TIMER_TMP
    PUSH  ZL
    PUSH  ZH

    IN   TIMER_TMP, PORTA
    ANDI TIMER_TMP, 0b11100000
    OUT  PORTA, TIMER_TMP
    ;OUT  PORTC, NULL

    CPI  DISP_CELL, 0
    BREQ d1
  CPI  DISP_CELL, 1
    BREQ d2
  CPI  DISP_CELL, 2
    BREQ d3
  CPI  DISP_CELL, 3
    BREQ d4

  RJMP OC0_EXIT

d1:
  IN   TIMER_TMP, PORTA
    ORI  TIMER_TMP, (1<<PA3)
    OUT  PORTA, TIMER_TMP

    MOV  TIMER_TMP, CURRENT_PIN1
    ANDI TIMER_TMP, 0x0F
    RJMP display_show_digit

d2:
  IN   TIMER_TMP, PORTA
    ORI  TIMER_TMP, (1<<PA2)
    OUT  PORTA, TIMER_TMP

    MOV  TIMER_TMP, CURRENT_PIN1
    LSR  TIMER_TMP
    LSR  TIMER_TMP
    LSR  TIMER_TMP
    LSR  TIMER_TMP
    RJMP display_show_digit

d3:
  IN   TIMER_TMP, PORTA
    ORI  TIMER_TMP, (1<<PA1)
    OUT  PORTA, TIMER_TMP

    MOV  TIMER_TMP, CURRENT_PIN2
    ANDI TIMER_TMP, 0x0F
    RJMP display_show_digit

d4:
  IN   TIMER_TMP, PORTA
    ORI  TIMER_TMP, (1<<PA0)
    OUT  PORTA, TIMER_TMP

    MOV  TIMER_TMP, CURRENT_PIN2
    LSR  TIMER_TMP
    LSR  TIMER_TMP
    LSR  TIMER_TMP
    LSR  TIMER_TMP
    RJMP display_show_digit

display_show_digit:
    LDI  ZL, low(SEG_TABLE*2)
    LDI  ZH, high(SEG_TABLE*2)
    ADD  ZL, TIMER_TMP
    BRCC DSH_NO_CARRY
    INC  ZH
DSH_NO_CARRY:
    LPM  TIMER_TMP, Z
    OUT  PORTC, TIMER_TMP
    RJMP OC0_EXIT

OC0_EXIT:
  INC DISP_CELL
  ANDI DISP_CELL, 0x03

  POP   ZH
    POP   ZL
    POP   TIMER_TMP
    OUT   SREG, TIMER_TMP             ; ??????????????? ?????
    POP   TIMER_TMP

  RETI


EEPROM_read:
    SBIC EECR, EEWE  
    RJMP EEPROM_read

    OUT EEARL, TMP
    OUT EEARH, NULL 
    SBI EECR, EERE 
    IN TMP2, EEDR

    RET

ADC_ISR:
  PUSH ADC_TMP
  IN ADC_TMP, SREG
  PUSH ADC_TMP

  CLR SECONDS_7
    IN   ADC_TMP, ADCH
    
    CPI  ADC_TMP, 25
    BRLO set_0

    CPI  ADC_TMP, 51
    BRLO set_1

    CPI  ADC_TMP, 76
    BRLO set_2

    CPI  ADC_TMP, 102
    BRLO set_3

  CPI  ADC_TMP, 127
    BRLO set_4

    CPI  ADC_TMP, 153
    BRLO set_5

    CPI  ADC_TMP, 178
    BRLO set_6

    CPI  ADC_TMP, 204
    BRLO set_7

  CPI  ADC_TMP, 229
    BRLO set_8

  RJMP set_9

set_0:
  LDI ADC_TMP, 0x00
  RJMP write_num_adc
set_1:
  LDI ADC_TMP, 0x01
  RJMP write_num_adc
set_2:
  LDI ADC_TMP, 0x02
  RJMP write_num_adc
set_3:
  LDI ADC_TMP, 0x03
  RJMP write_num_adc
set_4:
  LDI ADC_TMP, 0x04
  RJMP write_num_adc
set_5:
  LDI ADC_TMP, 0x05
  RJMP write_num_adc
set_6:
  LDI ADC_TMP, 0x06
  RJMP write_num_adc
set_7:
  LDI ADC_TMP, 0x07
  RJMP write_num_adc
set_8:
  LDI ADC_TMP, 0x08
  RJMP write_num_adc
set_9:
  LDI ADC_TMP, 0x09

write_num_adc:
    CPI CURRENT_CELL, 3
    BRPL set_34_adc
  
  CPI CURRENT_CELL, 2
  BREQ shift_number_adc

write_12_adc:
  CPI CURRENT_CELL, 1
  BREQ andi_cur1
  BRNE andi_cur2

set_after_andi_12:
    OR CURRENT_PIN1, ADC_TMP
    RJMP adc_EXIT
set_34_adc: 
    CPI CURRENT_CELL, 4
  BREQ shift_number_adc

write_34_adc:
  CPI CURRENT_CELL, 3
  BREQ andi_cur3
  BRNE andi_cur4

set_after_andi_34:
    OR CURRENT_PIN2, ADC_TMP
    RJMP adc_EXIT

andi_cur1:
  ANDI CURRENT_PIN1, 0xF0
  RJMP set_after_andi_12
andi_cur2:
  ANDI CURRENT_PIN1, 0x0F
  RJMP set_after_andi_12

andi_cur3:
  ANDI CURRENT_PIN2, 0xF0
  RJMP set_after_andi_34
andi_cur4:
  ANDI CURRENT_PIN2, 0x0F
  RJMP set_after_andi_34

shift_number_adc:
    LSL ADC_TMP
    LSL ADC_TMP
    LSL ADC_TMP
    LSL ADC_TMP

  CPI CURRENT_CELL, 3
    BRPL write_34_adc

  RJMP write_12_adc

adc_EXIT:
    SBI  ADCSRA, ADSC

  POP ADC_TMP
    OUT  SREG, ADC_TMP
    POP  ADC_TMP
    RETI

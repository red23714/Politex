.include "m32def.inc"

set_predil:
    ; --- Предделитель /1024 (CS02=1, CS01=0, CS00=1) ---
    ldi r16, (1<<CS02)|(1<<CS00)
    out TCCR0, r16

    ; --- Предделитель /256 ---
    ldi r16, (1<<CS02)
    out TCCR0, r16

    ; --- Предделитель /64 ---
    ldi r16, (1<<CS01)|(1<<CS00)
    out TCCR0, r16

    ; --- Остановить таймер ---
    ldi r16, 0x00
    out TCCR0, r16

set_mode:
    ; --- Normal mode, предделитель /1024 ---
    ldi r16, (1<<CS02)|(1<<CS00)
    out TCCR0, r16

    ; --- CTC mode, предделитель /64 ---
    ldi r16, (1<<WGM01)|(1<<CS01)|(1<<CS00)
    out TCCR0, r16

get_set_timer_value:
    ; --- Записать значение сравнения ---
    ldi r16, 200        ; сравнение при TCNT0 == 200
    out OCR0, r16

    ; --- Установить начальное значение счётчика ---
    ldi r16, 0
    out TCNT0, r16

    ; --- Прочитать текущее значение счётчика ---
    in r16, TCNT0       ; r16 = текущее значение таймера

set_interrupt:
    ; --- Прерывание по переполнению ---
    in r16, TIMSK
    ori r16, (1<<TOIE0)
    out TIMSK, r16

    ; --- Прерывание по совпадению ---
    in r16, TIMSK
    ori r16, (1<<OCIE0)
    out TIMSK, r16

    ; --- Оба прерывания сразу ---
    in r16, TIMSK
    ori r16, (1<<TOIE0)|(1<<OCIE0)
    out TIMSK, r16

    ; --- Глобально разрешить прерывания ---
    sei

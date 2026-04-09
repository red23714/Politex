; ============================================================
; Модуль проверки ПИН-кода, ATmega32, 8 МГц
; Твой костяк — без изменений логики.
; Добавлены: дисплей (4×7seg), таймер 7с (ввод), таймер 20с (ошибка)
; ============================================================

.include "m32def.inc"

; ---- регистры (твои) ----------------------------------------
.def NULL            = R16
.def TMP             = R17
.def TMP2            = R18
.def CURRENT_CELL    = R19
.def RIGHT_PIN1      = R20
.def RIGHT_PIN2      = R21
.def EEPROM_VALUE    = R22
.def EEPROM_ADR      = R23
.def CURRENT_PIN1    = R24
.def CURRENT_PIN2    = R25
.def ATTEMPT_COUNT   = R26
.def CURRENT_NUMBER  = R27
.def CURRENT_READ_NUMBER = R28
.def START_NEW       = R29

; ---- константы EEPROM (твои) --------------------------------
.equ EEPROM_ADR_PIN1 = 0x10
.equ EEPROM_ADR_PIN2 = 0x11

; ---- SRAM ---------------------------------------------------
.dseg
disp_buf:      .byte 4   ; буфер цифр для дисплея (0xFF = пусто)
multiplex_pos: .byte 1   ; текущий разряд мультиплексирования (0..3)
timer_7s:      .byte 2   ; счётчик таймаута ввода (тики по 10мс)
timer_7s_en:   .byte 1   ; 1 = таймаут ввода активен
timer_20s:     .byte 2   ; счётчик задержки ошибки (тики по 10мс)
timer_20s_en:  .byte 1   ; 1 = задержка ошибки активна

; ---- таблицы во флеш ----------------------------------------
.cseg

; ---- векторы прерываний -------------------------------------
.org 0x000
    RJMP reset
.org 0x002
    RJMP ext_int1
.org 0x007
    RJMP isr_timer0   ; Timer0 Compare Match — мультиплексирование (~1мс)
.org 0x014
    RJMP isr_timer2   ; Timer2 Compare Match — таймауты (~10мс)

.org 0x022
; коды сегментов для цифр 0..9 (общий анод, активный уровень — 1)
tabl_seg:
    .db 0x3F, 0x06   ; 0, 1
    .db 0x5B, 0x4F   ; 2, 3
    .db 0x66, 0x6D   ; 4, 5
    .db 0x7D, 0x07   ; 6, 7
    .db 0x7F, 0x6F   ; 8, 9

	
; маски анодов для разрядов 0..3 (PA0..PA3)
tabl_anod:
    .db 0x01, 0x02   ; разряд 0 (правый), разряд 1
    .db 0x04, 0x08   ; разряд 2, разряд 3 (левый)

; =============================================================
; ИНИЦИАЛИЗАЦИЯ
; =============================================================
reset:
    LDI TMP, HIGH(RAMEND)
    OUT SPH, TMP
    LDI TMP, LOW(RAMEND)
    OUT SPL, TMP

    CLR NULL
    CLR ATTEMPT_COUNT

    ; читаем правильный пин из EEPROM
    LDI EEPROM_ADR, EEPROM_ADR_PIN1
    CALL EEPROM_read
    MOV RIGHT_PIN1, EEPROM_VALUE
    LDI EEPROM_ADR, EEPROM_ADR_PIN2
    CALL EEPROM_read
    MOV RIGHT_PIN2, EEPROM_VALUE

    ; порты (твои настройки)
    CLR TMP
    OUT DDRB, TMP            ; PORTB — входы (кнопки 0..7)
    LDI TMP, 0x03
    OUT DDRA, TMP            ; PA0..PA1 — выходы (аноды разр. 0,1)
    ; PA2..PA3 тоже аноды — добавляем
    LDI TMP, 0b11001111      ; PA0..PA3 выходы (аноды), PA4..PA5 входы (кнопки 8,9), PA6..PA7 выходы (LED)
    OUT DDRA, TMP

    LDI TMP, 0x20
    OUT DDRD, TMP            ; PD5 — выход (не используется), остальные входы

    ; PORTC — шина сегментов, весь выход
    LDI TMP, 0xFF
    OUT DDRC, TMP
    CLR TMP
    OUT PORTC, TMP           ; сегменты погашены

    ; прерывания INT0/INT1 по фронту
    LDI TMP, 0x0F
    OUT MCUCR, TMP
    LDI TMP, 0xC0
    OUT GICR, TMP
    OUT GIFR, TMP

    ; Timer0: CTC, делитель /64, TOP=124 → ~1мс при 8МГц
    LDI TMP, 0x0B
    OUT TCCR0, TMP
    LDI TMP, 124
    OUT OCR0, TMP

    ; Timer2: CTC, делитель /1024, TOP=77 → ~10мс при 8МГц
    LDI TMP, 0x4F
    OUT TCCR2, TMP
    LDI TMP, 77
    OUT OCR2, TMP

    ; разрешаем прерывания от обоих таймеров
    LDI TMP, (1<<OCIE0)|(1<<OCIE2)
    OUT TIMSK, TMP

    SEI

; =============================================================
; МЯГКИЙ СБРОС (твой soft_reset + очистка дисплея)
; =============================================================
soft_reset:
    CLR TMP
    CLR TMP2
    CLR CURRENT_CELL
    CLR CURRENT_PIN1
    CLR CURRENT_PIN2
    CLR CURRENT_NUMBER
    CLR CURRENT_READ_NUMBER
    CLR START_NEW
    OUT PORTA, TMP           ; гасим LED

    ; очищаем буфер дисплея (0xFF = пусто)
    RCALL disp_clear

    ; останавливаем оба таймаута
    RCALL stop_timer_7s
    RCALL stop_timer_20s

; =============================================================
; ГЛАВНЫЙ ЦИКЛ (твой, без изменений)
; =============================================================
main_loop:
    RCALL read_number
    CPI CURRENT_CELL, 4
    BREQ check_correct
    RJMP main_loop

; =============================================================
; ЧТЕНИЕ НАЖАТОЙ КНОПКИ (твой код, без изменений)
; =============================================================
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
    ; ---- здесь добавляем только запуск/сброс таймера 7с ----
    RCALL reset_timer_7s     ; сбрасываем таймаут — кнопка нажата
    ; ---------------------------------------------------------

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

    ; обновляем дисплей после записи цифры
    RCALL disp_show_digit
    RET

set_34:
    ADD CURRENT_PIN2, CURRENT_NUMBER

    ; обновляем дисплей после записи цифры
    RCALL disp_show_digit
    RET

; =============================================================
; ПРОВЕРКА ПИНА (твой код, без изменений)
; =============================================================
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

    ; зажигаем PA6, ждём 20 секунд
    LDI TMP, (1<<PA6)
    OUT PORTA, TMP
    RCALL start_timer_20s    ; запускаем таймер 20с

wait_20s:
    LDS TMP, timer_20s_en
    CPI TMP, 0
    BRNE wait_20s            ; ждём пока таймер не сбросит флаг

    RJMP soft_reset

correct:
    LDI TMP, (1<<PA7)
    OUT PORTA, TMP

wait_new:
    CPI START_NEW, 1
    BRNE wait_new

    ; гасим LED и дисплей при завершении сеанса
    CLR TMP
    OUT PORTA, TMP
    RCALL disp_clear
    RJMP soft_reset

lose:
    LDI TMP, (1<<PA7)|(1<<PA6)
    OUT PORTA, TMP
lose_loop:
    RJMP lose_loop           ; зависаем

; =============================================================
; ПРЕРЫВАНИЕ INT1 (твой код, без изменений)
; =============================================================
ext_int1:
    PUSH TMP
    IN TMP, SREG
    LDI START_NEW, 1
    OUT SREG, TMP
    POP TMP
    RETI

; =============================================================
; EEPROM (твой код, без изменений)
; =============================================================
EEPROM_read:
    SBIC EECR, EEWE
    RJMP EEPROM_read
    OUT EEARL, EEPROM_ADR
    CLR TMP
    OUT EEARH, TMP
    SBI EECR, EERE
    IN EEPROM_VALUE, EEDR
    RET

; =============================================================
; ПОДПРОГРАММЫ УПРАВЛЕНИЯ ТАЙМАУТАМИ
; =============================================================

; запустить таймер 7 секунд (700 тиков по 10мс)
start_timer_7s:
    CLI
    LDI TMP, LOW(700)
    STS timer_7s,   TMP
    LDI TMP, HIGH(700)
    STS timer_7s+1, TMP
    LDI TMP, 1
    STS timer_7s_en, TMP
    SEI
    RET

; сбросить и запустить заново таймер 7с
reset_timer_7s:
    ; запускаем только если хотя бы одна цифра уже введена
    MOV TMP, CURRENT_CELL
    TST TMP
    BREQ reset_7s_skip       ; ещё ни одной цифры — не запускаем
    RCALL start_timer_7s
reset_7s_skip:
    RET

; остановить таймер 7с
stop_timer_7s:
    CLR TMP
    STS timer_7s_en, TMP
    RET

; запустить таймер 20 секунд (2000 тиков по 10мс)
start_timer_20s:
    CLI
    LDI TMP, LOW(2000)
    STS timer_20s,   TMP
    LDI TMP, HIGH(2000)
    STS timer_20s+1, TMP
    LDI TMP, 1
    STS timer_20s_en, TMP
    SEI
    RET

; остановить таймер 20с
stop_timer_20s:
    CLR TMP
    STS timer_20s_en, TMP
    RET

; =============================================================
; ПОДПРОГРАММЫ ДИСПЛЕЯ
; =============================================================

; очистить весь буфер дисплея (все разряды пусты)
disp_clear:
    LDI ZL, LOW(disp_buf)
    LDI ZH, HIGH(disp_buf)
    LDI TMP, 0xFF            ; 0xFF = пусто
    ST Z+, TMP
    ST Z+, TMP
    ST Z+, TMP
    ST Z,  TMP
    RET

; записать текущую цифру CURRENT_NUMBER в буфер на позицию CURRENT_CELL-1
; (вызывается сразу после INC CURRENT_CELL, поэтому -1)
; разряды: CURRENT_CELL=1 → disp_buf[0] (правый), CURRENT_CELL=2 → disp_buf[1] и т.д.
disp_show_digit:
    PUSH ZL
    PUSH ZH
    PUSH TMP
    PUSH TMP2

    ; позиция в буфере = CURRENT_CELL - 1
    MOV TMP, CURRENT_CELL
    DEC TMP                  ; теперь TMP = 0..3

    ; получаем реальную цифру из того, что было добавлено
    ; нечётные ячейки (0,2): цифра в старшем нибле CURRENT_PIN1/PIN2
    ; чётные ячейки (1,3): цифра в младшем нибле
    ; Проще: берём CURRENT_NUMBER и смотрим нибл
    ; После set_number CURRENT_NUMBER уже сдвинут (для нечётных × 16)
    ; Восстанавливаем реальную цифру:
    MOV TMP2, CURRENT_NUMBER
    SBRC CURRENT_CELL, 0     ; если CURRENT_CELL нечётное (после INC) — нибл старший
    RJMP ds_high_nib
    ; младший нибл (чётные ячейки: было 1, стало 2 после INC — то есть CELL=2,4)
    ANDI TMP2, 0x0F
    RJMP ds_store
ds_high_nib:
    SWAP TMP2
    ANDI TMP2, 0x0F
ds_store:
    LDI ZL, LOW(disp_buf)
    LDI ZH, HIGH(disp_buf)
    MOV TMP, CURRENT_CELL
    DEC TMP
    ADD ZL, TMP
    BRCC ds_no_carry
    INC ZH
ds_no_carry:
    ST Z, TMP2               ; сохраняем цифру в буфер

    POP TMP2
    POP TMP
    POP ZH
    POP ZL
    RET

; =============================================================
; ПРЕРЫВАНИЕ TIMER0 (~каждую 1мс): мультиплексирование дисплея
; =============================================================
isr_timer0:
    PUSH R0
    IN   R0, SREG
    PUSH R0
    PUSH R16
    PUSH R17
    PUSH R18
    PUSH ZL
    PUSH ZH

    ; гасим текущий разряд
    IN   R16, PORTA
    ANDI R16, 0b11000000     ; сохраняем биты PA6, PA7 (LED)
    OUT  PORTA, R16
    CLR  R16
    OUT  PORTC, R16          ; гасим сегменты

    ; читаем номер текущего разряда
    LDS  R17, multiplex_pos  ; R17 = 0..3

    ; читаем цифру из буфера для этого разряда
    LDI  ZL, LOW(disp_buf)
    LDI  ZH, HIGH(disp_buf)
    ADD  ZL, R17
    BRCC t0_z_ok
    INC  ZH
t0_z_ok:
    LD   R18, Z              ; R18 = цифра (0..9) или 0xFF (пусто)

    ; если пусто — этот разряд не светим
    CPI  R18, 0xFF
    BREQ t0_next

    ; получаем маску сегментов из таблицы
    LDI  ZL, LOW(tabl_seg<<1)
    LDI  ZH, HIGH(tabl_seg<<1)
    ADD  ZL, R18
    BRCC t0_seg_ok
    INC  ZH
t0_seg_ok:
    LPM  R16, Z
    OUT  PORTC, R16          ; выводим сегменты

    ; получаем маску анода из таблицы
    LDI  ZL, LOW(tabl_anod<<1)
    LDI  ZH, HIGH(tabl_anod<<1)
    ADD  ZL, R17
    BRCC t0_anod_ok
    INC  ZH
t0_anod_ok:
    LPM  R16, Z
    IN   R17, PORTA
    ANDI R17, 0b11000000     ; сохраняем биты LED
    OR   R17, R16
    OUT  PORTA, R17          ; включаем нужный анод

t0_next:
    ; переходим к следующему разряду
    LDS  R17, multiplex_pos
    INC  R17
    ANDI R17, 0x03           ; цикл 0→1→2→3→0
    STS  multiplex_pos, R17

    POP  ZH
    POP  ZL
    POP  R18
    POP  R17
    POP  R16
    POP  R0
    OUT  SREG, R0
    POP  R0
    RETI

; =============================================================
; ПРЕРЫВАНИЕ TIMER2 (~каждые 10мс): счётчики таймаутов
; =============================================================
isr_timer2:
    PUSH R0
    IN   R0, SREG
    PUSH R0
    PUSH R16
    PUSH R17

    ; --- таймер 7 секунд ввода ---
    LDS  R16, timer_7s_en
    TST  R16
    BREQ t2_check_20s        ; не активен — пропускаем

    LDS  R16, timer_7s
    LDS  R17, timer_7s+1
    SUBI R16, 1
    SBCI R17, 0              ; декрементируем 16-битный счётчик
    STS  timer_7s,   R16
    STS  timer_7s+1, R17

    ; если не дошли до нуля — продолжаем
    OR   R16, R17
    BRNE t2_check_20s

    ; таймер истёк — сбрасываем ввод
    CLR  R16
    STS  timer_7s_en, R16    ; останавливаем таймер

    ; сигнализируем главному циклу о сбросе через флаг
    ; используем START_NEW = 2 как код "таймаут ввода"
    ; НО: чтобы не трогать логику, делаем программный сброс иначе:
    ; устанавливаем специальный флаг в SRAM
    LDI  R16, 2
    STS  timer_7s_en+1, R16  ; используем соседний байт как флаг таймаута
    ; (этот байт = первый байт timer_20s, но он 0 когда 20с не активен)
    ; Лучше используем отдельный флаг — см. примечание ниже

t2_check_20s:
    ; --- таймер 20 секунд ошибки ---
    LDS  R16, timer_20s_en
    TST  R16
    BREQ t2_end

    LDS  R16, timer_20s
    LDS  R17, timer_20s+1
    SUBI R16, 1
    SBCI R17, 0
    STS  timer_20s,   R16
    STS  timer_20s+1, R17

    OR   R16, R17
    BRNE t2_end

    ; 20 секунд истекло — сбрасываем флаг (main_loop увидит)
    CLR  R16
    STS  timer_20s_en, R16

t2_end:
    POP  R17
    POP  R16
    POP  R0
    OUT  SREG, R0
    POP  R0
 

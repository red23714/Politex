include "m32def.inc"          ; подключаем описание регистров ATmega32

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
    RJMP  initializaciya       ; при включении идём на инициализацию
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

; читаем все 4 цифры из EEPROM в массив pin_cifry
zagruzit_pin_iz_eeprom:
    LDI   ZL, LOW(pin_cifry)   ; Z указывает на начало массива
    LDI   ZH, HIGH(pin_cifry)
    LDI   rC, 0                ; начинаем с адреса 0 в EEPROM
zagruzka_cikl:
    RCALL chtenie_eeprom       ; читаем байт из EEPROM в rA
    CPI   rA, 0xFF             ; 0xFF = ячейка никогда не записывалась
    BRNE  zagruzka_ok
    LDI   rA, 0                ; пустая ячейка — ставим 0
zagruzka_ok:
    ST    Z+, rA               ; записываем цифру и двигаем указатель
    INC   rC                   ; следующий адрес EEPROM
    CPI   rC, 4                ; загрузили все 4 цифры?
    BRNE  zagruzka_cikl        ; нет — повторяем
    RET

; настраиваем всё железо и переменные перед запуском
initializaciya:
    LDI   rA, HIGH(RAMEND)
    OUT   SPH, rA              ; настраиваем стек — старший байт
    LDI   rA, LOW(RAMEND)
    OUT   SPL, rA              ; настраиваем стек — младший байт

    SER   rA
    OUT   DDRC, rA             ; порт C весь на выход — шина сегментов дисплея
    CLR   rA
    OUT   PORTC, rA            ; гасим все сегменты

    LDI   rA, 0b00111111
    OUT   DDRA, rA             ; PA0..5 на выход, PA6..7 на вход (кнопки 8,9)
    CLR   rA
    OUT   PORTA, rA            ; всё выключено

    CLR   rA
    OUT   DDRB, rA             ; порт B весь на вход — кнопки 0..7
    OUT   PORTB, rA            ; подтяжки выключены

    CLR   rA
    OUT   DDRD, rA             ; порт D весь на вход — кнопки назад/вперёд
    OUT   PORTD, rA            ; подтяжки выключены

    CLR   rA                   ; обнуляем все переменные
    STS   tekush_poz,     rA
    STS   mig_schetchik,  rA
    STS   mig_sostoyanie, rA
    STS   taymaut_ml,     rA
    STS   taymaut_st,     rA
    STS   taymaut_vkl,    rA
    STS   multipleks,     rA

    IN    rA, PINB             ; запоминаем начальное состояние портов
    STS   prev_portB, rA       ; чтобы не поймать ложные нажатия при старте
    IN    rA, PINA
    STS   prev_portA, rA
    IN    rA, PIND
    STS   prev_portD, rA

    RCALL zagruzit_pin_iz_eeprom  ; загружаем сохранённый пин из EEPROM

    LDI   rA, 0x0B
    OUT   TCCR0, rA            ; таймер 0: режим CTC, делитель /64
    LDI   rA, 124
    OUT   OCR0, rA             ; TOP=124, при 8МГц даёт прерывание каждую мс

    LDI   rA, 0x4F
    OUT   TCCR2, rA            ; таймер 2: режим CTC, делитель /1024
    LDI   rA, 77
    OUT   OCR2, rA             ; TOP=77, даёт ~100 прерываний в секунду

    LDI   rA, (1<<OCIE2)|(1<<OCIE0)
    OUT   TIMSK, rA            ; разрешаем прерывания от обоих таймеров
    SEI                        ; включаем прерывания глобально

; главный цикл — опрашиваем кнопки
glavniy_cikl:
    ; читаем порт B (кнопки 0..7)
    IN    rA, PINB
    LDS   rB, prev_portB
    STS   prev_portB, rA       ; обновляем предыдущее состояние
    COM   rB
    AND   rA, rB               ; оставляем только биты которые только что стали 1
    BREQ  opros_portA          ; ничего нового — идём дальше

    CLR   rC                   ; начинаем искать с кнопки 0
poisk_knopki_pb:
    SBRC  rA, 0                ; нулевой бит = 1?
    RJMP  nazhata_knopka_pb    ; да — эта кнопка нажата
    LSR   rA                   ; сдвигаем маску вправо
    INC   rC                   ; смотрим следующую кнопку
    CPI   rC, 8
    BRNE  poisk_knopki_pb      ; не просмотрели все 8 — продолжаем
    RJMP  opros_portA

nazhata_knopka_pb:
    RCALL vvod_cifry           ; вводим цифру rC

    ; читаем порт A (кнопки 8 и 9 на битах 6 и 7)
opros_portA:
    IN    rA, PINA
    LDS   rB, prev_portA
    STS   prev_portA, rA
    COM   rB
    AND   rA, rB               ; только новые нажатия
    ANDI  rA, 0b11000000       ; нас интересуют только биты 6 и 7
    BREQ  opros_portD

    SBRC  rA, 6
    RJMP  knopka8              ; бит 6 — кнопка 8
    SBRC  rA, 7
    RJMP  knopka9              ; бит 7 — кнопка 9
    RJMP  opros_portD

knopka8:
    LDI   rC, 8
    RCALL vvod_cifry
    RJMP  opros_portD
knopka9:
    LDI   rC, 9
    RCALL vvod_cifry

    ; читаем порт D (кнопки назад/вперёд)
opros_portD:
    IN    rA, PIND
    LDS   rB, prev_portD
    STS   prev_portD, rA
    COM   rB
    AND   rA, rB               ; только новые нажатия
    SBRC  rA, 2
    RCALL shag_nazad           ; бит 2 — кнопка назад
    SBRC  rA, 3
    RCALL shag_vpered          ; бит 3 — кнопка вперёд
    RJMP  glavniy_cikl

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

; переходим на одну позицию назад
shag_nazad:
    LDS   rA, tekush_poz
    TST   rA
    BREQ  shag_nazad_konec     ; уже на первой позиции — некуда идти

    DEC   rA
    STS   tekush_poz, rA       ; двигаемся назад
    RCALL sbros_miganiya
    RCALL sbros_taymaut        ; сбрасываем таймаут

    LDS   rA, tekush_poz
    TST   rA
    BRNE  nazad_vkl_taymaut    ; не на первой позиции — включаем таймаут
    RET                        ; на первой — таймаут не нужен

nazad_vkl_taymaut:
    LDI   rA, 1
    STS   taymaut_vkl, rA      ; включаем таймаут
shag_nazad_konec:
    RET

; переходим на одну позицию вперёд
shag_vpered:
    LDS   rA, tekush_poz
    CPI   rA, 3                ; уже на последней позиции?
    BRNE  vpered_ne_posl
    RCALL sohranit_pin         ; да — считаем ввод завершённым
    RJMP  zavershenie

vpered_ne_posl:
    INC   rA
    STS   tekush_poz, rA       ; двигаемся вперёд
    RCALL sbros_miganiya
    RCALL sbros_taymaut
    LDI   rA, 1
    STS   taymaut_vkl, rA      ; включаем таймаут
    RET

; записываем все 4 цифры из массива в EEPROM
sohranit_pin:
    LDI   ZL, LOW(pin_cifry)
    LDI   ZH, HIGH(pin_cifry)
    LDI   rC, 0                ; начинаем с адреса 0 в EEPROM
sohranit_cikl:
    LD    rA, Z+               ; читаем цифру из массива
    RCALL zapis_eeprom         ; пишем в EEPROM
    INC   rC                   ; следующая ячейка
    CPI   rC, 4                ; записали все 4?
    BRNE  sohranit_cikl
    RET

; читаем байт из EEPROM по адресу rC, результат в rA
chtenie_eeprom:
    SBIC  EECR, EEWE
    RJMP  PC-1                 ; ждём пока прошлая запись не закончится
    OUT   EEARL, rC            ; задаём адрес
    SBI   EECR, EERE           ; запускаем чтение
    IN    rA, EEDR             ; забираем прочитанный байт
    RET

; пишем байт rA в EEPROM по адресу rC
zapis_eeprom:
    CLI                        ; запрещаем прерывания на время записи
    SBIC  EECR, EEWE
    RJMP  PC-1                 ; ждём пока прошлая запись не закончится
    OUT   EEARL, rC            ; задаём адрес
    OUT   EEDR, rA             ; кладём данные
    SBI   EECR, EEMWE          ; мастер-разрешение (обязательно перед EEWE)
    SBI   EECR, EEWE           ; запускаем запись
    SEI                        ; возвращаем прерывания
    RET

; ввод завершён — останавливаем всё и зависаем
zavershenie:
    RCALL sbros_taymaut        ; таймаут больше не нужен
    CLR   rA
    STS   mig_sostoyanie, rA   ; курсор гасим насовсем
petlya_konec:
    RJMP  petlya_konec         ; бесконечный цикл — ждём сброса

; прерывание таймера 0 (~каждую мс): переключаем разряды дисплея
prervanie_taimer0:
    PUSH  r0
    IN    r0, SREG             ; сохраняем флаги
    PUSH  r0
    PUSH  rA
    PUSH  rB
    PUSH  rC
    PUSH  ZL
    PUSH  ZH

    IN    rA, PORTA
    ANDI  rA, 0b11000000       ; сохраняем только биты кнопок (PA6, PA7)
    OUT   PORTA, rA            ; гасим аноды — выключаем текущий разряд
    CLR   rA
    OUT   PORTC, rA            ; гасим сегменты

    LDS   rB, multipleks       ; какой разряд сейчас показываем

    LDI   ZL, LOW(pin_cifry)
    LDI   ZH, HIGH(pin_cifry)
    ADD   ZL, rB               ; смещаемся к нужной цифре
    BRCC  t0_z_ok
    INC   ZH
t0_z_ok:
    LD    rC, Z                ; читаем цифру для этого разряда

    LDS   rA, tekush_poz
    CP    rB, rA               ; это активный (редактируемый) разряд?
    BRNE  t0_vyvesti           ; нет — выводим без проверки мигания
    LDS   rA, mig_sostoyanie
    TST   rA
    BRNE  t0_sleduyushiy       ; фаза "скрыть" — ничего не выводим

t0_vyvesti:
    LDI   ZL, LOW(tabl_segmentov<<1)  ; таблица во флеше — нужен сдвиг <<1
    LDI   ZH, HIGH(tabl_segmentov<<1)
    ADD   ZL, rC               ; смещаемся к нужной цифре
    BRCC  t0_segm
    INC   ZH
t0_segm:
    LPM   rA, Z                ; читаем маску сегментов из флеша
    OUT   PORTC, rA            ; выводим на дисплей

    LDI   ZL, LOW(tabl_razryadov<<1)
    LDI   ZH, HIGH(tabl_razryadov<<1)
    ADD   ZL, rB               ; смещаемся к нужному разряду
    BRCC  t0_razr
    INC   ZH
t0_razr:
    LPM   rA, Z                ; читаем маску анода из флеша
    IN    rC, PORTA
    ANDI  rC, 0b11000000       ; не трогаем биты кнопок
    OR    rC, rA               ; добавляем нужный анод
    OUT   PORTA, rC            ; включаем разряд

t0_sleduyushiy:
    LDS   rB, multipleks
    INC   rB
    ANDI  rB, 0x03             ; цикл 0→1→2→3→0
    STS   multipleks, rB       ; сохраняем номер следующего разряда

    POP   ZH
    POP   ZL
    POP   rC
    POP   rB
    POP   rA
    POP   r0
    OUT   SREG, r0             ; восстанавливаем флаги
    POP   r0
    RETI

; прерывание таймера 2 (~100 раз/с): мигание и таймаут
prervanie_taimer2:
    PUSH  r0
    IN    r0, SREG
    PUSH  r0
    PUSH  rA
    PUSH  rB

    ; мигание: каждые 25 тиков (~250мс) переключаем видимость курсора
    LDS   rA, mig_schetchik
    INC   rA
    CPI   rA, 25               ; прошло 25 тиков?
    BRNE  t2_schetchik_ok
    CLR   rA
    STS   mig_schetchik, rA    ; сбрасываем счётчик
    LDS   rB, mig_sostoyanie
    LDI   rA, 1
    EOR   rB, rA               ; инвертируем: 0→1 или 1→0
    STS   mig_sostoyanie, rB
    RJMP  t2_taymaut
t2_schetchik_ok:
    STS   mig_schetchik, rA    ; ещё не 25 — просто сохраняем

    ; таймаут бездействия: если нет нажатий ~3 секунды — сбрасываем ввод
t2_taymaut:
    LDS   rA, taymaut_vkl
    TST   rA
    BREQ  t2_konec             ; таймаут выключен — пропускаем

    LDS   rA, taymaut_ml       ; увеличиваем 16-битный счётчик
    LDS   rB, taymaut_st
    INC   rA
    BRNE  t2_taymaut_ok        ; не было переноса — старший байт не трогаем
    INC   rB                   ; был перенос — увеличиваем старший байт

t2_taymaut_ok:
    STS   taymaut_ml, rA
    STS   taymaut_st, rB

    CPI   rB, HIGH(300)        ; сравниваем старший байт с 300
    BRLO  t2_konec             ; ещё не дошли
    BRNE  t2_sbros             ; перескочили — сбрасываем
    CPI   rA, LOW(300)         ; старший совпал — смотрим младший
    BRLO  t2_konec             ; ещё не дошли

t2_sbros:                      ; прошло 300 тиков (~3 секунды) — сбрасываем
    CLR   rA
    STS   taymaut_vkl, rA      ; выключаем таймаут
    STS   taymaut_ml,  rA      ; обнуляем счётчик
    STS   taymaut_st,  rA

    PUSH  rC
    RCALL zagruzit_pin_iz_eeprom  ; восстанавливаем пин из EEPROM
    POP   rC

    CLR   rA                   ; возвращаем всё в начальное состояние
    STS   tekush_poz,     rA   ; курсор на первую позицию
    STS   mig_schetchik,  rA
    STS   mig_sostoyanie, rA
    LDI   rA, 1

    IN    rA, PINB             ; обновляем снимок портов
    STS   prev_portB, rA       ; чтобы не было ложных нажатий после сброса
    IN    rA, PINA
    STS   prev_portA, rA
    IN    rA, PIND
    STS   prev_portD, rA
t2_konec:
    POP   rB
    POP   rA
    POP   r0
    OUT   SREG, r0
    POP   r0
    RETI

tabl_segmentov:                ; коды сегментов для цифр 0..9
    .db 0x3F, 0x06             ; 0, 1
    .db 0x5B, 0x4F             ; 2, 3
    .db 0x66, 0x6D             ; 4, 5
    .db 0x7D, 0x07             ; 6, 7
    .db 0x7F, 0x6F             ; 8, 9
    .db 0x00, 0x00             ; пустые (на всякий случай)

tabl_razryadov:                ; маски анодов для каждого из 4 разрядов
    .db 0x01, 0x02             ; разряд 0, разряд 1
    .db 0x04, 0x08             ; разряд 2, разряд 3

#!/bin/bash
# performance_test_10000.sh

echo "=== ТЕСТ ПРОИЗВОДИТЕЛЬНОСТИ (10,000 ИТЕРАЦИЙ) ==="
echo ""

# 1. Очистка и компиляция
echo "Компиляция..."
make clear > /dev/null 2>&1
make qr_encoder qr_decoder > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "Ошибка компиляции!"
    exit 1
fi
echo "Готово"
echo ""

# Создаем временные файлы
TEMP_ENCODED="temp_encoded.lzqr"
TEMP_OUTPUT="temp_output.ppm"
TEMP_DECODED="temp_decoded.txt"

# Функция для измерения памяти одной команды
measure_memory() {
    local cmd="$1"
    # Используем /usr/bin/time для измерения пиковой памяти (RSS)
    /usr/bin/time -f "%M" $cmd 2>&1 | tail -n 1 | grep -o '^[0-9]\+'
}

# Функция для измерения времени выполнения
measure_time() {
    local cmd="$1"
    local start end
    start=$(date +%s%N)
    $cmd > /dev/null 2>&1
    end=$(date +%s%N)
    echo $(( (end - start) / 1000000 )) # в миллисекундах
}

# Инициализация переменных для средних значений
ENCODER_TIME_SUM=0
ENCODER_MEMORY_SUM=0
DECODER_TIME_SUM=0
DECODER_MEMORY_SUM=0
QR_DECODER_TIME_SUM=0
QR_DECODER_MEMORY_SUM=0

ITERATIONS=10000
SAMPLE_RATE=1000 # Измеряем память каждые N итераций

echo "Запуск тестов (${ITERATIONS} итераций)..."
echo "Прогресс:"

# 2. Тестирование qr_encoder (кодирование)
echo -n "qr_encoder (кодирование): "
for ((i=1; i<=$ITERATIONS; i++)); do
    # Показываем прогресс каждые 500 итераций
    if (( i % 500 == 0 )); then
        echo -n "."
    fi
    
    # Измеряем время каждой итерации
    time_ms=$(measure_time "./qr_encoder -c input.txt $TEMP_ENCODED --password 1234")
    ENCODER_TIME_SUM=$((ENCODER_TIME_SUM + time_ms))
    
    # Измеряем память каждые SAMPLE_RATE итераций для ускорения
    if (( i % SAMPLE_RATE == 0 || i == 1 )); then
        memory_kb=$(measure_memory "./qr_encoder -c input.txt $TEMP_ENCODED --password 1234")
        ENCODER_MEMORY_SUM=$((ENCODER_MEMORY_SUM + memory_kb))
    fi
    
    # Очистка временных файлов
    rm -f $TEMP_ENCODED
done
echo " Готово"

# 3. Тестирование qr_encoder (декодирование)
echo -n "qr_encoder (декодирование): "
# Нужно сначала закодировать файл
./qr_encoder -c input.txt $TEMP_ENCODED --password 1234 > /dev/null 2>&1

for ((i=1; i<=$ITERATIONS; i++)); do
    if (( i % 500 == 0 )); then
        echo -n "."
    fi
    
    time_ms=$(measure_time "./qr_encoder -d $TEMP_ENCODED $TEMP_OUTPUT --password 1234")
    DECODER_TIME_SUM=$((DECODER_TIME_SUM + time_ms))
    
    if (( i % SAMPLE_RATE == 0 || i == 1 )); then
        memory_kb=$(measure_memory "./qr_encoder -d $TEMP_ENCODED $TEMP_OUTPUT --password 1234")
        DECODER_MEMORY_SUM=$((DECODER_MEMORY_SUM + memory_kb))
    fi
    
    rm -f $TEMP_OUTPUT
done
echo " Готово"

# 4. Тестирование qr_decoder
echo -n "qr_decoder: "
# Нужно сначала создать PPM файл
./qr_encoder -d $TEMP_ENCODED $TEMP_OUTPUT --password 1234 > /dev/null 2>&1

for ((i=1; i<=$ITERATIONS; i++)); do
    if (( i % 500 == 0 )); then
        echo -n "."
    fi
    
    time_ms=$(measure_time "./qr_decoder $TEMP_OUTPUT 1234")
    QR_DECODER_TIME_SUM=$((QR_DECODER_TIME_SUM + time_ms))
    
    if (( i % SAMPLE_RATE == 0 || i == 1 )); then
        memory_kb=$(measure_memory "./qr_decoder $TEMP_OUTPUT 1234")
        QR_DECODER_MEMORY_SUM=$((QR_DECODER_MEMORY_SUM + memory_kb))
    fi
done
echo " Готово"

# Расчет средних значений
ENCODER_AVG_TIME=$(echo "scale=2; $ENCODER_TIME_SUM / $ITERATIONS" | bc)
ENCODER_MEMORY_SAMPLES=$((ITERATIONS / SAMPLE_RATE + 1))
ENCODER_AVG_MEMORY=$(echo "scale=0; $ENCODER_MEMORY_SUM / $ENCODER_MEMORY_SAMPLES" | bc)

DECODER_AVG_TIME=$(echo "scale=2; $DECODER_TIME_SUM / $ITERATIONS" | bc)
DECODER_MEMORY_SAMPLES=$((ITERATIONS / SAMPLE_RATE + 1))
DECODER_AVG_MEMORY=$(echo "scale=0; $DECODER_MEMORY_SUM / $DECODER_MEMORY_SAMPLES" | bc)

QR_DECODER_AVG_TIME=$(echo "scale=2; $QR_DECODER_TIME_SUM / $ITERATIONS" | bc)
QR_DECODER_MEMORY_SAMPLES=$((ITERATIONS / SAMPLE_RATE + 1))
QR_DECODER_AVG_MEMORY=$(echo "scale=0; $QR_DECODER_MEMORY_SUM / $QR_DECODER_MEMORY_SAMPLES" | bc)

# Очистка временных файлов
rm -f $TEMP_ENCODED $TEMP_OUTPUT $TEMP_DECODED

# 5. Вывод результатов
echo ""
echo "==============================================="
echo "     РЕЗУЛЬТАТЫ ИЗМЕРЕНИЙ (10,000 ИТЕРАЦИЙ)"
echo "==============================================="
echo ""
printf "%-35s %15s %15s\n" "ПРОГРАММА" "СРЕДНЕЕ ВРЕМЯ" "СРЕДНЯЯ ПАМЯТЬ"
printf "%-35s %15s %15s\n" "" "(мс)" "(KB)"
echo "------------------------------------------------"
printf "%-35s %15.2f %15s\n" "qr_encoder (кодирование)" "$ENCODER_AVG_TIME" "$ENCODER_AVG_MEMORY"
printf "%-35s %15.2f %15s\n" "qr_encoder (декодирование)" "$DECODER_AVG_TIME" "$DECODER_AVG_MEMORY"
printf "%-35s %15.2f %15s\n" "qr_decoder" "$QR_DECODER_AVG_TIME" "$QR_DECODER_AVG_MEMORY"
echo "==============================================="

# Дополнительная статистика
echo ""
echo "ДОПОЛНИТЕЛЬНАЯ СТАТИСТИКА:"
echo "----------------------------"
echo "Всего выполнено итераций: $ITERATIONS"
echo "Замеров памяти выполнено: $ENCODER_MEMORY_SAMPLES на модуль"
echo "Суммарное время тестирования: ~$(echo "scale=0; ($ENCODER_TIME_SUM + $DECODER_TIME_SUM + $QR_DECODER_TIME_SUM) / 1000" | bc) секунд"

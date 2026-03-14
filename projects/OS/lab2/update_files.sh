#!/bin/bash

# Конфигурация
SOURCE_DIR="./build"  # Директория с исходными файлами (корневая папка)
TARGET_DIR="./ibks"  # Директория, где искать папки folder_*
TARGET_PATTERN="folder_*"  # Шаблон для поиска папок
FILES=("tcpclient" "tcpserver" "udpclient" "udpserver")  # Файлы для копирования

# Цвета для вывода
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Функция вывода с цветом
print_status() {
    local color=$1
    local message=$2
    echo -e "${color}${message}${NC}"
}

# Функция логирования
log_action() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" >> copy_log.txt
}

# Проверка существования папки tests
if [ ! -d "$TARGET_DIR" ]; then
    print_status "$RED" "Ошибка: Папка '$TARGET_DIR' не найдена!"
    exit 1
fi

# Проверка исходных файлов
print_status "$YELLOW" "Проверка исходных файлов..."
missing_files=0
for file in "${FILES[@]}"; do
    if [ -f "$SOURCE_DIR/$file" ]; then
        print_status "$GREEN" "  ✓ Найден: $file"
    else
        print_status "$RED" "  ✗ Отсутствует: $file"
        ((missing_files++))
    fi
done

if [ $missing_files -gt 0 ]; then
    print_status "$RED" "Ошибка: Отсутствует $missing_files файл(ов)"
    exit 1
fi

# Поиск целевых папок в ./tests/
print_status "$YELLOW" "\nПоиск папок по шаблону '$TARGET_DIR/$TARGET_PATTERN'..."
mapfile -t folders < <(find "$TARGET_DIR" -maxdepth 1 -type d -name "$TARGET_PATTERN" 2>/dev/null | sort)

if [ ${#folders[@]} -eq 0 ]; then
    print_status "$RED" "Папки не найдены в директории $TARGET_DIR!"
    print_status "$YELLOW" "Содержимое папки tests:"
    ls -la "$TARGET_DIR"
    exit 0
fi

print_status "$GREEN" "Найдено папок: ${#folders[@]}"
for folder in "${folders[@]}"; do
    echo "  - $(basename "$folder")"
done

# Запрос подтверждения
echo
read -p "Продолжить копирование? (y/n): " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    print_status "$YELLOW" "Операция отменена"
    exit 0
fi

# Копирование файлов
print_status "$YELLOW" "\nКопирование файлов..."
total_success=0
total_failed=0

for folder in "${folders[@]}"; do
    print_status "$YELLOW" "\n📁 $(basename "$folder")"
    
    for file in "${FILES[@]}"; do
        if cp "$SOURCE_DIR/$file" "$folder/" 2>/dev/null; then
            print_status "$GREEN" "  ✓ $file"
            log_action "COPIED: $file -> $(basename "$folder")/"
            ((total_success++))
        else
            print_status "$RED" "  ✗ $file (ошибка копирования)"
            log_action "FAILED: $file -> $(basename "$folder")/ - Ошибка копирования"
            ((total_failed++))
        fi
    done
done

# Итоговый отчет
print_status "$GREEN" "\n✅ Копирование завершено!"
echo "Успешно скопировано: $total_success"
echo "Ошибок: $total_failed"
echo "Лог сохранен в: copy_log.txt"

if [ $total_failed -eq 0 ]; then
    print_status "$GREEN" "Все файлы успешно скопированы во все папки!"
else
    print_status "$RED" "Некоторые файлы не скопировались. Проверьте лог."
fi

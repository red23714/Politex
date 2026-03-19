#!/bin/bash

# =========================================
# Конфигурация
# =========================================
SOURCE_DIR="./build"   # Папка с бинарниками (4 файла)
TARGET_DIR="./ibks"    # Папка с глобальными поддиректориями (tcpclient, udpclient, ...)
BINS=("tcpclient" "tcpserver" "udpclient" "udpserver")  # Бинарники

# Цвета
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_status() {
    local color=$1
    local message=$2
    echo -e "${color}${message}${NC}"
}

log_action() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" >> copy_log.txt
}

# =========================================
# Удаление всех бинарников из ibks рекурсивно
# =========================================
print_status "$YELLOW" "Удаляем старые бинарники из $TARGET_DIR..."
for bin in "${BINS[@]}"; do
    find "$TARGET_DIR" -type f -name "$bin" -exec rm -f {} \;
done
print_status "$GREEN" "Старые бинарники удалены"

# =========================================
# Проверка бинарников
# =========================================
print_status "$YELLOW" "Проверка бинарников в $SOURCE_DIR..."
missing_files=0
for bin in "${BINS[@]}"; do
    if [ ! -f "$SOURCE_DIR/$bin" ]; then
        print_status "$RED" "  ✗ Отсутствует: $bin"
        ((missing_files++))
    else
        print_status "$GREEN" "  ✓ Найден: $bin"
    fi
done

if [ $missing_files -gt 0 ]; then
    print_status "$RED" "Ошибка: отсутствует $missing_files бинарник(ов)!"
    exit 1
fi

# =========================================
# Копирование бинарников
# =========================================
print_status "$YELLOW" "\nКопирование бинарников по категориям..."

# Находим глобальные папки (tcpclient, udpclient, tcpserver, udpserver)
for global_folder in "$TARGET_DIR"/*; do
    if [ -d "$global_folder" ]; then
        global_name=$(basename "$global_folder")

        # Проверяем, есть ли бинарник с таким же именем
        if [[ ! " ${BINS[@]} " =~ " ${global_name} " ]]; then
            print_status "$YELLOW" "Пропускаем папку $global_name — нет соответствующего бинарника"
            continue
        fi

        bin_to_copy="$SOURCE_DIR/$global_name"
        print_status "$YELLOW" "📁 Копируем в $global_name/*"

        # Копируем бинарник во все folder_*
        mapfile -t folders < <(find "$global_folder" -type d -name "folder_*" | sort)

        for folder in "${folders[@]}"; do
            if cp "$bin_to_copy" "$folder/"; then
                print_status "$GREEN" "  ✓ $global_name -> $(basename "$folder")"
                log_action "COPIED: $global_name -> $(basename "$folder")/"
            else
                print_status "$RED" "  ✗ $global_name -> $(basename "$folder") (ошибка)"
                log_action "FAILED: $global_name -> $(basename "$folder")/"
            fi
        done
    fi
done

print_status "$GREEN" "\n✅ Копирование завершено!"
echo "Лог сохранен в copy_log.txt"

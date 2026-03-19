#!/bin/bash

set -e

ROOT=$(pwd)
TESTS_DIR="$ROOT/ibks"
LOGS_DIR="$ROOT/logs"
TIMEOUT_SEC=60  # ограничение времени выполнения run.rb

mkdir -p "$LOGS_DIR"

# перебираем подпапки tcpclient, tcpserver, udpclient, udpserver и т.д.
for group_folder in "$TESTS_DIR"/*; do
    if [ -d "$group_folder" ]; then
        group_name=$(basename "$group_folder")
        echo "=== Обработка группы $group_name ==="

        for folder in "$group_folder"/folder_*; do
            if [ -d "$folder" ]; then
                folder_name=$(basename "$folder")
                echo "=== Запуск тестов в $folder_name ($group_name) ==="
                cd "$folder"

                # создаём папку для логов конкретной папки
                folder_logs="$LOGS_DIR/$group_name/$folder_name"
                mkdir -p "$folder_logs"

                # Запуск run.rb с таймаутом
                echo "  ruby run.rb ..."
                if ! timeout "$TIMEOUT_SEC"s ruby run.rb; then
                    echo "  Внимание: run.rb превысил $TIMEOUT_SEC секунд, прерываем."
                fi

                # Перемещаем stdout и логи
                mv *.stdout "$folder_logs/" 2>/dev/null || true
                mv run.log "$folder_logs/" 2>/dev/null || true
                mv msg.txt "$folder_logs/" 2>/dev/null || true

                echo "  Логи сохранены в $folder_logs"

                cd "$ROOT"
            fi
        done
    fi
done

echo "=== Все тесты завершены ==="

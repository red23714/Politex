#!/bin/bash

set -e

ROOT=$(pwd)
TESTS_DIR="$ROOT/tests"
LOGS_DIR="$ROOT/logs"

mkdir -p "$LOGS_DIR"

for folder in "$TESTS_DIR"/folder_*; do
    if [ -d "$folder" ]; then
        echo "=== Запуск тестов в $(basename $folder) ==="
        cd "$folder"

        # Создаём папку для логов конкретной папки
        folder_logs="$LOGS_DIR/$(basename $folder)"
        mkdir -p "$folder_logs"

        # Запуск run.rb
        echo "  ruby run.rb ..."
        ruby run.rb

        # Перемещаем stdout и логи
        mv *.stdout "$folder_logs/" 2>/dev/null || true
        mv run.log "$folder_logs/" 2>/dev/null || true
        mv msg.txt "$folder_logs/" 2>/dev/null || true

        echo "  Логи сохранены в $folder_logs"

        cd "$ROOT"
    fi
done

echo "=== Все тесты завершены ==="

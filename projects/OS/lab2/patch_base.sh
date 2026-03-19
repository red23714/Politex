#!/usr/bin/env bash
ROOT="./ibks"

# Рекурсивно ищем все файлы base.rb внутри ROOT
find "$ROOT" -type f -name "base.rb" | while read -r f; do
  # Исправляем Ruby синтаксис
  sed -i 's/File\.exists?/File.exist?/g' "$f"
  echo "✅ Fixed $f"
done

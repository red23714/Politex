#!/usr/bin/env bash
ROOT="./tests"

find "$ROOT" -type f -name "base.rb" | while read f; do
  sed -i 's/File\.exists?/File.exist?/g' "$f"
  echo "✅ Fixed $f"
done

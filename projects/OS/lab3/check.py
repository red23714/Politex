import subprocess
import glob
import os
import shutil


def is_sorted(filename):
    with open(filename, "r") as f:
        lines = f.readlines()

    # пропускаем первые 2 строки
    data = lines[2:]

    # собираем все числа
    numbers = []
    for line in data:
        numbers.extend(map(int, line.split()))

    return all(numbers[i] <= numbers[i + 1] for i in range(len(numbers) - 1))


def main():
    input_files = sorted(glob.glob("tests/input_*.txt"))

    if not input_files:
        print("Нет входных файлов")
        return

    for file in input_files:
        print(f"Обработка {file}...")

        # копируем входной файл
        shutil.copy(file, "input.txt")

        # запускаем бинарник
        result = subprocess.run(["build/msort"])

        if result.returncode != 0:
            print(f"{file}: ошибка выполнения")
            continue

        if not os.path.exists("output.txt"):
            print(f"{file}: output.txt не найден")
            continue

        if is_sorted("output.txt"):
            print(f"{file}: OK")
        else:
            print(f"{file}: FAIL")


if __name__ == "__main__":
    main()

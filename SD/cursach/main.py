import random

def generate_random_sequence():
    letters = ['1', '2', '3']  # Используемые символы
    length = 100000  # Общее количество символов
    # Генерация последовательности
    sequence = [random.choice(letters) for _ in range(length)]
    return ''.join(sequence)

# Генерация последовательности
random_sequence = generate_random_sequence()

# Запись в файл
with open('file.txt', 'w', encoding='utf-8') as file:
    file.write(random_sequence)

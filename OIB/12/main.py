import os
import ctypes

def main():
    file_path = "C:/Users/red/Desktop/misha.txt"
    
    print("1 - Архивный файл\n2 - Скрыть файл\n3 - Для чтения и записи\n4 - Только для чтения\n5 - Cоздать файл\n6 - Записать в файл\n7 - Прочитать из файла")
    
    try:
        n = int(input())
    except ValueError:
        print("Ошибка: введите число от 1 до 7")
        return
    
    if n == 1:
        # Установка атрибута "Архивный"
        try:
            ctypes.windll.kernel32.SetFileAttributesW(file_path, 0x20)  # FILE_ATTRIBUTE_ARCHIVE
            print("Файл помечен как архивный")
        except:
            print("Ошибка при установке атрибута")
    
    elif n == 2:
        # Установка атрибута "Скрытый"
        try:
            ctypes.windll.kernel32.SetFileAttributesW(file_path, 0x2)  # FILE_ATTRIBUTE_HIDDEN
            print("Файл скрыт")
        except:
            print("Ошибка при установке атрибута")
    
    elif n == 3:
        # Установка нормального атрибута
        try:
            ctypes.windll.kernel32.SetFileAttributesW(file_path, 0x80)  # FILE_ATTRIBUTE_NORMAL
            print("Файл доступен для чтения и записи")
        except:
            print("Ошибка при установке атрибута")
    
    elif n == 4:
        # Установка атрибута "Только для чтения"
        try:
            ctypes.windll.kernel32.SetFileAttributesW(file_path, 0x1)  # FILE_ATTRIBUTE_READONLY
            print("Файл доступен только для чтения")
        except:
            print("Ошибка при установке атрибута")
    
    elif n == 5:
        # Создание файла
        try: 
            with open(file_path, '+a'):
                pass
                print("Файл создан")
        except Exception as e:
            print("Ошибка при создании файла", str(e))
    
    elif n == 6:
        # Запись в файл
        try:
            if os.path.exists(file_path):
                if not os.access(file_path, os.W_OK):
                    print("Файл только для чтения")
                else:
                    content = input("Введите текст для записи: ")
                    with open(file_path, 'w', encoding='utf-8') as f:
                        f.write(content)
                    print("Текст записан в файл")
            else:
                print("Файл не существует")
        except:
            print("Ошибка при записи в файл")
    
    elif n == 7:
        # Чтение из файла
        try:
            if os.path.exists(file_path):
                with open(file_path, 'r', encoding='utf-8') as f:
                    content = f.read()
                    print("Содержимое файла:")
                    print(content)
            else:
                print("Файл не существует")
        except:
            print("Ошибка при чтении файла")
    
    else:
        print("Неверный выбор. Введите число от 1 до 7")

main()
while True:
    pass

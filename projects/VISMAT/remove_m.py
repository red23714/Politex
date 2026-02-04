def remove_carriage_returns(filename):
    """
    Удаляет символы ^M (возврат каретки \r) из файла
    """
    try:
        # Читаем файл
        with open(filename, 'r', encoding='utf-8') as file:
            content = file.read()
        
        # Удаляем символы возврата каретки
        cleaned_content = content.replace('\r', '')
        
        # Записываем обратно
        with open(filename, 'w', encoding='utf-8') as file:
            file.write(cleaned_content)
        
        print(f"Символы ^M успешно удалены из файла {filename}")
        
    except FileNotFoundError:
        print(f"Файл {filename} не найден")
    except Exception as e:
        print(f"Произошла ошибка: {e}")

# Использование
remove_carriage_returns('math_integrator.h')

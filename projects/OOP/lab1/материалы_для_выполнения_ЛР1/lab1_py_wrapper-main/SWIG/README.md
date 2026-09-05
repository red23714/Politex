# Создание Python-обертки для C++ класса при помощи SWIG
Simplified Wrapper and size_terface Generator (SWIG) поддерживает множество языков, включая Python, и автоматически генерирует обертки для C++ классов и функций.

Документация: https://www.swig.org/doc.html

## Пример С++ класса
Файл example.h:
```c++
#ifndef EXAMPLE_H
#define EXAMPLE_H

class Example {
public:
    Example(int value) : value(value) {}

    int getValue() const {
        return value;
    }

    void setValue(int v) {
        value = v;
    }

private:
    int value;
};

#endif
```
Файл example.cpp:
```c++
#include "example.h"
```

## Шаги создания Python-обертки
1. Установка SWIG: 
    - Windows: 
        1. cкачать zip-файл и распаковать его: http://www.swig.org/ или напрямую https://sourceforge.net/projects/swig/
        2. создать переменную среды окружения: https://www.swig.org/Doc1.3/Windows.html#Windows_examples
    - Linux: 
        - установить с помощью пакетного менеджера: `sudo apt-get install swig`
    - macOS: 
        - установить с помощью пакетного менеджера brew: `brew install swig`
2. Создание интерфейсного SWIG-файла:
    Этот файл служит связующим звеном между C++ и Python, определяя, какие классы и функции будут доступны в Python.
    
    Файл example.i:

    ```c++
    %module example
    %{
        #include "example.h"
    %}

    // Указание классов и методов для генерации оберток
    %include "example.h"
    ```
    где:
    - %module example — указывает имя модуля Python, который будет создан
    - %{ #include "example.h" %} — включает оригинальный заголовочный файл C++
    - %include "example.h" — сообщает SWIG, что нужно сгенерировать обертку для содержимого файла example.h

    Расширить интерфейс класса или уточнить поведение при вызове методов можно через дополнительную секции %extend

3. Компиляция модуля:
    1. Запуск SWIG
        Для генерации C++ оберток запустим SWIG: `swig -python -c++ example.i`. Результат:
        - example_wrap.cxx - автоматически сгенерированный файл обертки для Python
        - example.py — интерфейсный Python-файл для модуля
    2. Компиляция C++:
    - **Windows, используя Visual Studio**:
        1. Откройте Developer Command Prompt for Visual Studio
        2. Перейдите в каталог, где находятся файлы example.h, example.cpp и example_wrap.cxx.
        3. Выполните команду для компиляции:
        
            ```bat
            cl /LD /I C:\path\to\python\include example_wrap.cxx example.cpp /link /LIBPATH:C:\path\to\python\libs python38.lib /OUT:_example.pyd
            ```
            где:
            - /LD — указывает, что мы создаём динамическую библиотеку (DLL).
            - /I C:\path\to\python\include — путь к заголовочным файлам Python.
            - example_wrap.cxx example.cpp — исходные файлы C++ и сгенерированный файл SWIG.
            - /link /LIBPATH:C:\path\to\python\libs — путь к библиотекам Python.
            - python38.lib — библиотека Python для версии 3.8 (замените на соответствующую библиотеку, если используете другую версию Python).
            - /OUT:_example.pyd — имя выходного файла (в Windows используется расширение .pyd для модулей Python).
    - **Linux** / **macOS**:
        1. Открыть терминал.
        2. Перейти в каталог, где находятся файлы example.cpp, example.h и example_wrap.cxx.
        3. Выполнить команду для компиляции (на примере `clang++`):

            ```bash
            clang++ -shared -fPIC -std=c++17 example_wrap.cxx example.cpp `python3-config --includes --ldflags --embed` -o _example`python3-config --extension-suffix`
            ```
            где:
            - -shared — создание динамическую библиотеку.
            - -fPIC — использование позиционно-независимый код.
            - -std=c++17 - задание версии спецификации C++ 2017.
            - example_wrap.cxx — файл, сгенерированный SWIG.
            - example.cpp — оригинальный файл с кодом C++.
            - python3-config:
                - --includes - возвращает перечень путей до директорий с заголовками Python (.h). Пример:
                    
                    ```bash
                    -I/Library/Frameworks/Python.framework/Versions/3.11/include/python3.11 -I/Library/Frameworks/Python.framework/Versions/3.11/include/python3.11
                    ```
                - --ldflags --embed - возвращает пути до директорий с библиотеками и перечень библиотек. Пример:

                    ```bash
                    -L/Library/Frameworks/Python.framework/Versions/3.11/lib/python3.11/config-3.11-darwin -lpython3.11 -ldl -framework CoreFoundation
                    ```
                - --extension-suffix - возвращает суффикс имени расщирения. Пример: `.cpython-311-darwin.so`

    Результат компиляции:
    - Linux / macOS - файл _example.so
    - Windows - файл _example.pyd
    
4. Использование модуля

    ```python
    import example

    obj = example.Example(10)
    print(obj.getValue())  # Вывод: 10
    obj.setValue(20)
    print(obj.getValue())  # Вывод: 20
    ```
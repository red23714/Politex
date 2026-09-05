# Создание Python-обертки для C++ класса при помощи pybind11
pybind11 - библиотека, работающая только с заголовками, которая предоставляет типы C++ в Python и наоборот.

Документация: https://pybind11.readthedocs.io/en/stable/index.html

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
1. Установка pybind11: `pip install pybind11`
2. Создание файла с оберткой:
    
    Файл example_wrapper.cpp:

    ```c++
    #include <pybind11/pybind11.h>
    #include "example.h"

    namespace py = pybind11;

    PYBIND11_MODULE(example, m) {
        py::class_<Example>(m, "Example")
            .def(py::init<int>())  // Конструктор с параметром int
            .def("getValue", &Example::getValue)  // Метод getValue
            .def("setValue", &Example::setValue);  // Метод setValue
    }
    ```
3. Компиляция модуля:
    - **Windows, используя Visual Studio**:
        1. Откройте Developer Command Prompt for Visual Studio: `"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\vsdevcmd" -arch=amd64`, где `amd64` - требуемая архитектура.
        2. Перейдите в каталог, где находятся файлы example.cpp, example.h и example_wrapper.cpp.
        3. Получить пути до заголочных файлов pybind11 и Python: `py -m pybind11 --includes`. Пример: 

            ```bat
            C:\Users\user>py -m pybind11 --include
            -IC:\Users\user\AppData\Local\Programs\Python\Python311\Include -IC:\Users\user\AppData\Local\Programs\Python\Python311\Lib\site-packages\pybind11\include
            ```

            > **_ВАЖНО:_**  Имя переменной среды окружения для вызова Python может отличаться!
        3. Выполните команду для компиляции:

            ```bat
            cl /LD /I C:\path\to\pybind11 /I C:\path\to\python\include example_wrapper.cpp example.cpp /link /LIBPATH:C:\path\to\python\libs pythonXXX.lib /OUT:example.pyd
            ```
            где:
            - `/LD` — указывает, что мы собираем динамическую библиотеку (DLL).
            - `/I C:\path\to\pybind11` и `/I C:\path\to\python\include` — заменить на вывод команды, выполненной на шаге 3.
            - `/LIBPATH:C:\path\to\python\libs` — путь к библиотекам Python. Например: `C:\Users\user\AppData\Local\Programs\Python\Python311\libs`
            - `pythonXXX.lib` — библиотека Python X.XX. Например: `python311.lib`
            - /OUT:example.pyd — имя выходного файла (модуль Python).
    - **Linux** / **macOS**:
        1. Открыть терминал.
        2. Перейдите в каталог, где находятся файлы example.cpp, example.h и example_wrap.cxx.
        3. Получить пути до директорий с заголовочными файлами и библиотеками и Python: 
        
            ```bash
            clang++ -shared -fPIC -std=c++17 `python3-config --ldflags --embed` `python3 -m pybind11 --includes` example_wrapper.cpp example.cpp -o example`python3-config --extension-suffix`
            ```
            где:
            - -shared — создание динамическую библиотеку.
            - -fPIC — использование позиционно-независимый код.
            - -std=c++17 - задание версии спецификации C++ 2017.
            - example_wrapper.cpp — файл, содержащий обертку.
            - example.cpp — оригинальный файл с кодом C++.
            - `python3 -m pybind11 --includes` - возвращает перечень директорий с заголовками Python (.h). Пример:
               
                ```bash
                user@MacBook-Pro-User example % python3 -m pybind11 --includes
                -I/Library/Frameworks/Python.framework/Versions/3.11/include/python3.11 -I/Library/Frameworks/Python.framework/Versions/3.11/lib/python3.11/site-packages/pybind11/include
                ```
            - python3-config:
                - --ldflags --embed - возвращает пути до директорий с библиотеками и перечень библиотек. Пример:
                   
                    ```bash
                    -L/Library/Frameworks/Python.framework/Versions/3.11/lib/python3.11/config-3.11-darwin -lpython3.11 -ldl -framework CoreFoundation
                    ```
                - --extension-suffix - возвращает суффикс имени расщирения. Пример: `.cpython-311-darwin.so`

    Результат компиляции:
    - Linux / macOS - файл example.so
    - Windows - файл example.pyd
    
4. Использование модуля

    Версия интерпретатора Python должна совпадать с версией библиотеки, используемой при сборке Python-модуля (.pyd).

    ```python
    import example

    obj = example.Example(10)
    print(obj.getValue())  # Вывод: 10
    obj.setValue(20)
    print(obj.getValue())  # Вывод: 20
    ```
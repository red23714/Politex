# Создание Python-обертки для C++ класса при помощи Boost.Python
Boost.Python - это часть библиотеки Boost, которая позволяет интегрировать классы и функции на C++ с Python.

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
1. Установка Boost.Python: 
    - Windows: 
        - для самостоятельной сборки:
            1. cкачать: https://www.boost.org/users/download/
            2. cобрать бибилотеку boost.python (https://www.boost.org/doc/libs/1_52_0/more/getting_started/windows.html):
                - запустить `bootstrap.bat` для сборки утилиты `b2.exe`
                - перейти в директорию `libs\python\build` и выполнить команду `..\..\..\b2.exe runtime-link=static link=static release stage`, где `release` - тип сборки. По умолчанию используется `debug`
            3. изменить конфигурацию boost для сборки статического Python-модуля (`.pyd`):
                - в заголовок `boost/config/user.hpp` добавить флаг `#define BOOST_PYTHON_STATIC_LIB`
                - до строки подключения `boost/python.hpp` добавить флаг `#define BOOST_PYTHON_STATIC_LIB`
        - pre-built библиотека:
            Поставляется в виде динамических библиотек (`.dll`) и прокси статических библиотек (`.lib`). Для успешного импорта собранного модуля интерперетатором Python (`import example`) потребуется скопировать библиотеку boost-python*.dll в директорию с Python-модулем (`.pyd`).
            1. cкачать: https://sourceforge.net/projects/boost/files/boost-binaries/
            2. установить библиотеку при помощи загруженного инсталятора
    - Linux: 
        - вариант 1 - установить аналогично Windows
        - вариант 2 - установить с помощью пакетного менеджера: `sudo apt-get install libboost-python-dev`
    - macOS: 
        - установить с помощью пакетного менеджера brew: `brew install boost` и `brew install boost-python3`
2. Создание файла с оберткой:
    
    Файл example_wrapper.cpp:

    ```c++
    #include <boost/python.hpp>
    #include "example.h"

    using namespace boost::python;

    // Обёртка для класса Example
    BOOST_PYTHON_MODULE(example) {
        class_<Example>("Example", init<size_t>())  // Класс и его конструктор
            .def("getValue", &Example::getValue)  // Метод getValue
            .def("setValue", &Example::setValue);  // Метод setValue
    }
    ```
3. Компиляция модуля:
    - **Windows, используя Visual Studio**:
        1. Откройте Developer Command Prompt for Visual Studio: `"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\vsdevcmd" -arch=amd64`, где `amd64` - требуемая архитектура.
        2. Перейдите в каталог, где находятся файлы example.cpp, example.h и example_wrapper.cpp.
        3. Выполните команду для компиляции:
            - для boost.python собранного самостоятельно:
                ```bat
                cl /LD /MT /EHsc /I C:\path\to\boost /I C:\path\to\python\include example_wrapper.cpp example.cpp /link /LIBPATH:C:\path\to\boost\lib /LIBPATH:C:\path\to\python\libs boost_python312.lib python312.lib /OUT:example.pyd
                ```
                где:
                - `/LD` — указывает, что мы собираем динамическую библиотеку (.dll).
                - `/MT` - использование статической версии библиотеки runtime (.lib).
                - `/EHsc` - включает поддержку обработки исключений в стиле C++.
                - `/I C:\path\to\boost` — путь к заголовочным файлам Boost. Например: `C:\local\boost_1_86_0`
                - `/I C:\path\to\python\include` — путь к заголовочным файлам Python. Например: `C:\Users\user\AppData\Local\Programs\Python\Python312\include`
                - `/LIBPATH:C:\path\to\boost\lib` — путь к скомпилированным статическими библиотекам Boost. Например: `C:\Users\user\Downloads\boost_1_86_0\boost_1_86_0\stage\lib`
                - /LIBPATH:C:\path\to\python\libs — путь к библиотекам Python. Например: `C:\Users\user\AppData\Local\Programs\Python\Python312\libs`
                - `boost_python312.lib` — библиотека Boost.Python для Python 3.12. Например: `libboost_python312-vc143-mt-s-x64-1_86.lib`
                - `python312.lib` — библиотека Python 3.12.
                - `/OUT:example.pyd` — имя выходного файла (модуль Python).
            - для pre-built boost.python:
                ```bat
                cl /LD /MD /EHsc /I C:\path\to\boost /I C:\path\to\python\include example_wrapper.cpp example.cpp /link /LIBPATH:C:\path\to\boost\lib /LIBPATH:C:\path\to\python\libs boost_python312.lib python312.lib /OUT:example.pyd
                ```
                где:
                - `/MD` - использование динамической версии библиотеки runtime (.dll).
                - `/LIBPATH:C:\path\to\boost\lib` — путь к скомпилированным статическими библиотекам Boost. Например: `C:\local\boost_1_86_0\lib64-msvc-14.3`.
                - `boost_python312.lib` — библиотека Boost.Python для Python 3.12. Например: `boost_python312-vc143-mt-x64-1_86.lib`
    - **Linux** / **macOS**:
        1. Открыть терминал.
        2. Перейти в каталог, где находятся файлы example.cpp, example.h и example_wrapper.cpp.
        4. Выполнить команду для компиляции (на примере `clang++`):

            ```bash
            clang++ -shared -fPIC -std=c++17 example_wrapper.cpp example.cpp `python3-config --includes --ldflags --embed` -lboost_python312 -o example`python3-config --extension-suffix`
            ```
            где:
            - -shared — создание динамическую библиотеку.
            - -fPIC — использование позиционно-независимый код.
            - -std=c++17 - задание версии спецификации C++ 2017.
            - example_wrapper.cpp — файл, содержащий обертку.
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
    - Linux - файл example.so
    - Windows - файл example.pyd
    
4. Использование модуля
    
    Версия интерпретатора Python должна совпадать с версией библиотеки, используемой при сборке Python-модуля (.pyd).
    - **Windows**:
        При динамической компоновке Python-модуля (.pyd) необходимо в директорию с модулем скопировать boost_python*.dll.

    ```python
    import example

    obj = example.Example(10)
    print(obj.getValue())  # Вывод: 10
    obj.setValue(20)
    print(obj.getValue())  # Вывод: 20
    ```
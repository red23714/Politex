# Расширение функциональности интерпретатора Python с помощью класса C++.

## Библиотеки для реализации связи C++ и Python:
Библиотеки разделяются по времени компиляции С++ кода: 
1. Явная компиляция проекта С++ в динамическую библиотеку .so или .pyd в зависимсоти от ОС (explicit):
    - Boost.Python
    - pybind11
    - SWIG
2. Run-time компиляцию кода C++ (implicit):
    - cppyy

Таким образом, в случае использования `cppyy` при каждом запуске Python-приложения выполняется компиляция кода обертки класса C++.

## Инструкции

[Инструкция по использованию Boost.Python](Boost/README.md)

[Инструкция по использованию pybind11](pyBind11/README.md)

[Инструкция по использованию SWIG](SWIG/README.md)


## Документация интерфейса, предоставляемого Python

- Extending Python with C or C++¶: https://docs.python.org/3/extending/extending.html
- C API Stability: https://docs.python.org/3/c-api/stable.html#stable
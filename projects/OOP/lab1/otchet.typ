#import "@preview/lilaq:0.5.0" as lq

#set heading(numbering: "1.")
#show heading.where(level: 1): set text(size: 16pt)
#show heading.where(level: 2): set text(size: 14pt)

#set page(
  paper: "a4",
  margin: (
    top: 2.54cm,
    bottom: 2.54cm,
    right: 2.54cm,
    left: 2.54cm
  )
)

#set text(
  lang: "ru",
  font: "Times New Roman",
  size: 14pt,
)

#let par_indent = 1.5cm

#set list(
  indent: par_indent / 2,
  body-indent: par_indent / 2,
)

#set enum(
  indent: par_indent / 2,
  body-indent: par_indent / 2
)

#show heading: it => block(
  inset: (left: par_indent),
  spacing: 1em,
)[
  #it
]

#set par(
  justify: true,
  leading: 0.4em,
  first-line-indent: par_indent,
  spacing: 0.65em,
)

#let indented-heading(body) = {
  body
  h(par_indent)
}

#show heading: indented-heading

#show figure: it => block(
  spacing: 2em,
)[
  #it
]

#set rect(
  inset: 8pt,
  fill: rgb("ffff"),
  width: 100%,
)

#grid(
  rows: (0.6fr, 2fr, 2fr, 0fr),
  columns: (100%),
  gutter: 3pt,
  stroke: none,
  fill: none,
  rect[ 
    #align(center + top)[
      Министерство образования и науки Российской Федерации\
    ] 
    #align(right)[
      Санкт-Петербургский Политехнический Университет Петра Великого \
    ]
    #align(center)[
      — \
      Институт компьютерных наук и кибербезопасности
    ]
  ],
  [
    #align(center + horizon)[
      #text(size: 16pt)[*ЛАБОРАТОРНАЯ РАБОТА № 1*] \
      \
      *«Класс MyString»* \ 
      \
      по дисциплине «Объектно-ориентированное программирование»
    ]
  ],
  grid(
    rows: (0.5fr, 1fr),
    columns: (0.8fr, 0.7fr, 0.6fr),
    gutter: 3pt,
    [
      Выполнил \
      студент гр.5151001/40202
    ],
    [
      #align(center)[
        \
        #symbol("<")подпись#symbol(">")
      ]
    ],
    [
      #align(right)[Волошкевич М.А.]
    ],
    [
      Преподователь /\
      ассистент
    ],
    [
      #align(center)[
        \
        #symbol("<")подпись#symbol(">")
      ]
    ],
    [
      #align(right)[Семьянов П.В.]
    ]
  ),
  [
    #align(center + horizon)[
      Санкт-Петербург\
      2026г.
    ]
  ]
)

#pagebreak()

#set par(
  justify: true,
  leading: 0.8em,
  first-line-indent: par_indent,
  spacing: 0.7em,
)

= Цель работы.
Знакомство с базовыми понятиями объектноориентированного программирования «класс» и «объект» и принципом
инкапсуляции.

#pagebreak()

= Задачи
В рамках выполнения лабораторной работы были решены следующие задачи:
- реализовать класс для представления символьной строки на C++, не используя контейнеры и алгоритмы библиотеки STL;
- расширь функциональность интерпретатора Python с помощью реализованного класса.

#pagebreak()

= Ход работы
Класс MyString содержит в себе три поля: len\_, отвечающее за сохранения длины строки от нулевого индекса до нуль-терминатора, capacity\_, отвечающее за размер выделенной памяти под строку которая хранится, а также для формата хранения символов была выбрана си-строка в виде char\* указателя. Так же в лабараторной работе для передачи строк разного типа(std::string, const char\*, MyString) была использована структура std::string_view и был перегружен оператор неявного преобразования для класса, чтобы std::string_view поддерживал класс MyString для передачи в методы класса.

#pagebreak()

Ниже на @pic_decomp представлен результирующий граф полной декомпозиции класса MyString. 

#figure(
  image("./материалы_для_выполнения_ЛР1/build_call_graph-master/my_string_graph.png"),
  caption: [Граф алгоритмической декомпозиции] 
) <pic_decomp>

#pagebreak()

Реализация Python обертки была выполнена с помощью библиотеки Boost. В реализации была проблема с переносом структуры std::string_view, для библиотеки Boost нету готового преобразования, поэтому была реализована собственное преобразование std::string, const char\* и MyString в обычную си строку которая легко конвертируется из Python строки. Для остальных методов класса были переписаны объявления функций из заголовочного файла.

#pagebreak()

= Выводы
В ходе лабаратоной работы были изучены базовые понятия объектноориентированного программирования, реализован и протестирован класс MyString, а так же получены знания для расширения функциональности python с помощью кода на C++.

#pagebreak()

#set heading(numbering: none)
= Приложения

== Приложение А
\
main.cpp
```cpp

```
my_string.cpp
```cpp

```
my_sting.h
```cpp

```

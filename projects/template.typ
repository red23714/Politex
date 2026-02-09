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
      #text(size: 16pt)[*ЛАБОРАТОРНАЯ РАБОТА № 3*] \
      \
      *«Нахождение n-нного элемента последовательности»* \ 
      \
      по дисциплине «Структуры данных»
    ]
  ],
  grid(
    rows: (0.5fr, 1fr),
    columns: (0.8fr, 0.7fr, 0.6fr),
    gutter: 3pt,
    [
      Выполнил \
      студент гр.5151001/40001
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
      2025г.
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
Реализовать алгоритм для нахождения энного элемента в последовательности, состоящей из чисел, которые являются произведением степеней чисел 3, 5 и 7. Проверить корректность работы алгоритма и его эффективность.

= Постановка задачи
#lorem(50)

#lorem(30)

= Теоретические исследования
Для выполнения работы необходимо было изучить алгоритмы генерации последовательностей, а также методы динамического управления памятью для хранения промежуточных результатов. Основные теоретические аспекты:
- Последовательность формируется из чисел вида 3^a×5^b×7^c, где a, b, c — неотрицательные целые числа.
- Для эффективного нахождения следующего элемента используются указатели на текущие минимальные кандидаты для умножения на 3, 5 и 7.
- Динамический массив используется для хранения элементов последовательности с возможностью его компактификации для оптимизации использования памяти.

Говно список говна:
+ Fisrt
+ Second
+ Third


#lorem(70)


#figure(
  image("diagram.png", height: 60%),
  caption: [Блок схема работы программы]
) <pic1>

В @pic1 у нас находится блок схема с говном

#pagebreak()

#figure(
  table(
    columns: 2,
    [*Amount*], [*Ingredient*],
    [360g], [Baking flour],
    [250g], [Butter (room temp.)],
    [150g], [Brown sugar],
    [100g], [Cane sugar],
    [100g], [70% cocoa chocolate],
    [100g], [35-40% cocoa chocolate],
    [2], [Eggs],
    [Pinch], [Salt],
    [Drizzle], [Vanilla extract],
  ),
  caption: [Таблица с говном]
) <tab1>

В @tab1 какая-то залупа

#figure(
  lq.diagram(
    lq.plot((0, 1, 2, 3, 4), (3, 5, 4, 2, 3))
  ),
  caption: [График говна по говну]
) <pic2>

В @pic2 у нас какой-то график по состоянию говна на сегодняшний говеный день

#pagebreak()

#set heading(numbering: none)
= Приложения

== Приложение А
\
main.c
```c
#include <stdio.h>

void print_usage()
{
	printf("Usage:\n");
	printf("  Compression:   ./main -c <input_file> <output_file> --password "
		   "<password>\n");
	printf("  Decompression: ./main -d <input_file> <output_file> --password "
		   "<password>\n");
}

```

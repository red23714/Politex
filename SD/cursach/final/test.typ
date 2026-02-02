#set rect(
  inset: 8pt,
  fill: rgb("e4e5ea"),
  width: 100%,
  height: 100%
)

#set text(
  font: "Times New Roman",
  size: 14pt,
)

#set par(
  leading: 0.4em
)

#grid(
  rows: (0.6fr, 2fr, 2fr, 0.25fr),
  gutter: 3pt,
  [ 
    #align(center)[
      Министерство образования и науки Российской Федерации \
      Санкт-Петербургский Политехнический Университет Петра Великого \
      — \
      Институт кибербезопасности и защиты информации]
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
    rows: (0.3fr, 1fr),
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

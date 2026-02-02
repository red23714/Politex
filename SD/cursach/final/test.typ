#set rect(
  inset: 8pt,
  fill: rgb("e4e5ea"),
  width: 100%,
  height: 100%
)

#grid(
  rows: (0.5fr, 2fr, 2fr, 0.25fr),
  gutter: 3pt,
  rect[ 
    #align(center)[
      Министерство образования и науки Российской Федерации \
      Санкт-Петербургский Политехнический Университет Петра Великого \
      — \
      Институт кибербезопасности и защиты информации]
  ],
  rect[
    #align(center + horizon)[
      *ЛАБОРАТОРНАЯ РАБОТА № 3*

      *«Нахождение n-нного элемента последовательности»* \  
      по дисциплине «Структуры данных»
    ]
  ],
  grid(
    rows: (1fr, 1fr),
    columns: (1fr, 1fr, 1fr),
    gutter: 3pt,
    rect[
      Выполнил \
      студент гр.5151001/40001
    ],
    rect[
      #align(center)[
        \
        #symbol("<")подпись#symbol(">")
      ]
    ],
    rect[
      #align(right)[Волошкевич М.А.]
    ],
    rect[
      Преподователь /\
      ассистент
    ],
    rect[
      #align(center)[
        \
        #symbol("<")подпись#symbol(">")
      ]
    ],
    rect[
      #align(right)[Семьянов П.В.]
    ]
  ),
  rect(height: 100%)[
    #align(center + horizon)[
      Санкт-Петербург\
      2025г.
    ]
  ]
)

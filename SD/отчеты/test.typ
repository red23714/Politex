#set page(margin: 2cm)
#set text(font: ("Times New Roman", "Libertinus Serif"), size: 12pt)


#let header = {
  rect(
    width: 100%,
    height: 100%,
    stroke: none,
  )
}

#let middle = {
  rect(
    width: 100%,
    height: 100%,
    stroke: none,
  )
}

#let square = rect(
  width: 100%,
  height: 100%,
  stroke: none,
)

#let footer = {
  rect(
    width: 100%,
    height: 100%,
    stroke: none,
  )
}

#grid(
  columns: (1fr),
  rows: (15%, 30%, 40%, 10%),
  
  // Верхний прямоугольник с министерством
  place(1, 1)[
    #header
    #align(center + horizon, text(size: 11pt)[
      Министерство образования и науки Российской Федерации \
      Санкт-Петербургский Политехнический Университет Петра Великого \
      — \
      Институт кибербезопасности и защиты информации
    ])
  ],
  
  // Второй прямоугольник с темой работы
  place(1, 2)[
    #middle
    #align(center + horizon, text(size: 14pt, weight: "bold")[
      ЛАБОРАТОРНАЯ РАБОТА № 3
      
      #v(1.5em)
      
      «Нахождение n-ного элемента последовательности»
      
      #v(1em)
      
      по дисциплине «Структуры данных»
    ])
  ],
  
  // Три квадрата с информацией о выполнении
  place(1, 3)[
    #grid(
      columns: (1fr, 1fr, 1fr),
      gutter: 2cm,
      
      // Первый квадрат - "Выполнил"
      [#square
      #align(left, block(
        spacing: 1.5em,
        text(weight: "bold")[Выполнил],
        text[студент гр. 5151001/40001],
        text(weight: "medium")[Волошкевич М.А.],
        text(height: 2em)[``],
        text(style: "italic")[<подпись>]
      ))],

      // Второй квадрат - подпись студента (можно оставить пустым или добавить линию)
      [#square
      #align(center + horizon, block(
        spacing: 1em,
        text(height: 4em)[``], // место для подписи
        text(style: "italic")[Подпись студента]
      ))],

      // Третий квадрат - "Преподаватель"
      [#square
      #align(right, block(
        spacing: 1.5em,
        text(weight: "bold")[Преподаватель],
        text[Семьянов П.В.],
        text(height: 2em)[``],
        text(style: "italic")[<подпись>]
      ))]
    )
  ],
  
  // Нижний прямоугольник с городом и годом
  place(1, 4)[
    #footer
    #align(center + horizon, text(size: 11pt)[
      Санкт-Петербург \
      2025 г.
    ])
  ],
)
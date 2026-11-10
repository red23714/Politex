// ============================================================================
// Библиотека оформления отчёта по лабораторной работе.
// Повторяет форматирование «Рекомендации_по_написанию_отчёта_1.docx».
//
// Подключение в документе:
//   #import "lib.typ": *
//   #show: report
//
// Экспортируется:
//   • report(body)          — применить все правила оформления (#show: report)
//   • title-page(...)       — титульный лист (геометрия 1:1 с docx)
//   • listing(caption, b)   — листинг (рамка-таблица, подпись сверху слева)
//   • appendix(title)       — приложение («Приложение N. Название», справа)
//   • eq-plain(body)        — ненумеруемая формула
//   • where-par(body)       — абзац-пояснение «где …» после формулы (без отступа)
//   • landscape(body)       — альбомная страница для больших рисунков/таблиц
//   • enum-paren / enum-letters / dash-list — списки «1)», «а)», «–»
//   • th(body)              — ячейка шапки таблицы (по центру)
//   • tbl(table, caption)   — переносимая таблица с подписью (для длинных)
//   • параметры: font-main, par-indent, line-15, line-115, line-1,
//     empty-line-15, empty-line-115, empty-line-1
//
// ВАЖНО (отличие от прошлой версии): интервалы считаются в модели Word.
// Word при 14 pt Times New Roman: одинарный = 16.1 pt, 1.15 = 18.5 pt,
// 1.5 = 24.2 pt между базовыми линиями. В Typst строка по умолчанию
// считается от cap-height до baseline, поэтому «1.5» нельзя задать как
// 0.6em без настройки краёв строки — см. set text(top-edge:…) в report().
// ============================================================================

// ------------------------- параметры ----------------------------------------
// Times New Roman, при его отсутствии — метрически совместимый Liberation Serif
#let font-main = ("Times New Roman", "Liberation Serif")
#let par-indent = 1.25cm // красная строка (абзац 1.25)

// Межстрочные интервалы (leading) при краях строки 12.47pt/3.02pt (14 pt TNR):
// строка Typst = 15.5 pt, Word 1.5× = 24.2 pt → leading = 8.7 pt = 0.62em и т.д.
#let line-15 = 0.62em   // интервал 1.5  → 24.2 pt между базовыми линиями
#let line-115 = 0.215em // интервал 1.15 → 18.5 pt
#let line-1 = 0.05em    // интервал 1    → 16.2 pt

// Высоты «пустых строк» (в docx это реальные пустые абзацы)
#let empty-line-15 = 24.2pt  // пустая строка с интервалом 1.5
#let empty-line-115 = 18.5pt // пустая строка с интервалом 1.15
#let empty-line-1 = 16.1pt   // пустая строка с интервалом 1

// Отступы (above/below) для воспроизведения «пустой строки после текста 1.5».
// В Word зазор от базовой линии текста до следующего объекта = остаток строки
// 1.5 (24.1 pt) + высота пустой строки; в Typst строка даёт только 15.5 pt,
// поэтому к высоте пустой добавляется ~8.6 pt (цель — базовая линия)
// или ~8.0 pt (цель — верх блока: картинка).
#let gap-empty-1 = 24.7pt        // пустая строка 1.0 перед подписью таблицы/листинга
#let gap-empty-15 = 32.8pt       // пустая строка 1.5 перед заголовком (уровни 2–4)
#let gap-empty-15-block = 32.2pt // пустая строка 1.5 перед рисунком (цель — верх картинки)
#let gap-after-caption = 24.8pt  // пустая строка 1.5 после подписи (текст 1.0 → текст 1.5)

// Позиция колонки ФИО на титульном листе (левый табулятор docx: 6840 twips)
#let tab-fio = 12.06cm

// ------------------------- список «а)», «б)» ---------------------------------
// русские строчные буквы для списков вида «а)», «б)», «в)»
#let rus-lower = (n) => {
  let letters = (
    "а", "б", "в", "г", "д", "е", "ж", "з", "и", "к", "л", "м", "н", "о", "п",
    "р", "с", "т", "у", "ф", "х", "ц", "ч", "ш", "щ", "э", "ю", "я",
  )
  if n <= letters.len() { letters.at(n - 1) } else { "?" }
}

// Маркеры всех списков начинаются от красной строки (1.25 см), как в docx;
// body-indent подобран так, чтобы текст попадал на позиции табуляторов docx:
// «1.» → текст на 2.25 см от поля, «1)»/«а)» → 2.5 см, «–» → 2 см.
#let enum-letters = enum.with(numbering: (n) => rus-lower(n) + ")", indent: par-indent, body-indent: 0.84cm, spacing: line-15)
#let enum-paren = enum.with(numbering: "1)", indent: par-indent, body-indent: 0.84cm, spacing: line-15)
#let dash-list = list.with(indent: par-indent, body-indent: 0.5cm, spacing: line-15, marker: [–])

// Ячейка шапки таблицы — по центру (в docx «Заголовки выравнены по центру»).
// Пишите шапку так: table.header([], th[Столбец 1], th[Столбец 2]) —
// table.header заодно повторит шапку при переносе таблицы на новую страницу.
// Почему явно: show-правила по y/header внутри figure в Typst 0.13–0.15
// центрируют и остальные ячейки (наследуется выравнивание figure),
// поэтому надёжнее задать выравнивание каждой ячейке шапки.
#let th(body) = table.cell(align: center)[#body]

// ------------------------- листинг ------------------------------------------
// Рамка — таблица 1×1; код Courier New 12 pt, интервал 1.15 (15.6 pt между
// строками, как в docx), края строки заданы под Courier New (asc 0.833em,
// descender — ключевым словом: длины в bottom-edge Typst игнорирует),
// содержимое явно прижато влево, чтобы его не центрировало выравнивание figure
#let listing(caption, body) = figure(
  table(
    columns: (1fr,),
    stroke: 0.5pt,
    inset: (x: 5.4pt, y: 1.5pt), // стандартные поля ячеек Word: 0.19 см
    align(left)[
      // код удобно передавать raw-блоком (``` … ```): не нужно экранировать
      // скобки, звёздочки и решётки; шрифт raw принудительно Courier New
      #show raw: set text(font: ("Courier New", "Liberation Mono"))
      #set text(font: ("Courier New", "Liberation Mono"), size: 12pt, top-edge: 10pt, bottom-edge: "descender", hyphenate: false)
      #set par(leading: 3.3pt, spacing: 3.3pt, first-line-indent: 0pt, justify: false)
      #body
    ],
  ),
  caption: caption,
  kind: "listing",
)

// ------------------------- переносимые таблицы ------------------------------
// Таблица с подписью «Таблица N – Название» сверху слева, которая МОЖЕТ
// переноситься на следующую страницу (в docx большие таблицы переносят
// именно так, а шапку дублируют). figure(table(…)) в Typst атомарен —
// длинная таблица «уедет» целиком на новую страницу, поэтому для длинных
// таблиц используйте tbl(). Счётчик общий с figure-таблицами, нумерация
// сквозная. Шапку набирайте через table.header(th[…], …) — она повторится
// на каждой странице автоматически.
//
//   #tbl(
//     table(columns: (1fr, 1fr), table.header(th[А], th[Б]), [..], [..]),
//     [Название таблицы],
//   )
#let tbl(tbl-body, caption) = {
  counter(figure.where(kind: table)).step()
  context block(width: 100%, above: gap-empty-1, below: empty-line-15, breakable: true)[
    #set par(first-line-indent: 0pt, justify: false, leading: line-1, spacing: line-1)
    #align(left)[Таблица #counter(figure.where(kind: table)).display() – #caption]
    #tbl-body
  ]
}

// ------------------------- формулы ------------------------------------------
// ненумеруемая формула (если на неё нет ссылки в тексте)
#let eq-plain(body) = math.equation(block: true, numbering: none, body)

// абзац-пояснение после формулы: «где …» — в docx БЕЗ красной строки
#let where-par(body) = {
  set par(first-line-indent: 0pt)
  body
}

// ------------------------- альбомная страница --------------------------------
// Для больших рисунков и таблиц, как в docx: разрыв страницы, альбомная
// ориентация, поля: верх 3, низ 1, лево 2, право 2 (см).
#let landscape(body) = page(
  paper: "a4",
  flipped: true,
  margin: (top: 3cm, bottom: 1cm, left: 2cm, right: 2cm),
  body,
)

// ------------------------- приложения ---------------------------------------
#let appendix-counter = counter("appendix")

// «Приложение N. Название» — с новой страницы, по правому краю,
// жирным и капсом (в docx это стиль, унаследованный от «Заголовка 1»)
#let appendix(title) = {
  pagebreak(weak: true)
  appendix-counter.step()
  context {
    let n = appendix-counter.get().first()
    set par(first-line-indent: 0pt, justify: false, spacing: line-15, leading: line-15)
    align(right, text(font: font-main, size: 14pt, weight: "bold")[
      #upper[Приложение #n. #title]
    ])
  }
}

// ------------------------- титульный лист -----------------------------------
// Геометрия снята с docx (базовые линии рендера):
//   шапка 2.46 см; ЛАБОРАТОРНАЯ РАБОТА 12.72 см; по дисциплине 15.02 см;
//   Выполнил 18.88 см; Санкт-Петербург – год 27.10 см.
// Зазоры ниже — это v() между блоками: из расстояния Word вычтены
// нижний вынос строки предыдущего блока и верхний вынос следующего.
#let title-page(
  lab-no: "1",
  topic: [Название работы],
  discipline: [«Название дисциплины»],
  group: "ХХХХХХХ/ХХХХХ",
  student: "И.О. Фамилия",
  supervisor-role: "ассистент ВШК ИКНК",
  supervisor: "И.О. Фамилия",
  city: "Санкт-Петербург",
  year: 2025,
) = [
  #set par(first-line-indent: 0pt, justify: false, spacing: 0pt)

  // строка для ФИО: левая часть + фиксированная позиция 12.06 см (табулятор docx)
  #let fio-line(left-part, right-part) = grid(
    columns: (tab-fio, auto),
    align(left)[#left-part],
    align(left)[#right-part],
  )

  // ---- шапка: 4 строки, интервал 1.15, по центру ----
  #align(center)[
    #set par(leading: line-115)
    Министерство науки и высшего образования Российской Федерации\
    Санкт-Петербургский политехнический университет Петра Великого\
    Институт компьютерных наук и кибербезопасности\
    Высшая школа кибербезопасности
  ]

  // в docx: пустая строка 1.5 + 9 пустых строк 1.15 + пустая строка 1.5
  #v(217.9pt)

  // ---- название работы: 16 pt, жирный ----
  #align(center)[
    #set text(size: 16pt, weight: "bold")
    #set text(top-edge: 14.26pt) // 0.891em от 16 pt
    ЛАБОРАТОРНАЯ РАБОТА №#lab-no
  ]
  #v(9.8pt)
  #align(center)[
    #set text(size: 16pt, weight: "bold")
    #set text(top-edge: 14.26pt)
    #topic
  ]

  // в docx: пустая строка с интервалом 1.15
  #v(21.8pt)

  // ---- «по дисциплине» / «…»: 14 pt, интервал 1.15 ----
  #align(center)[
    #set par(leading: line-115)
    по дисциплине\
    #discipline
  ]

  // в docx: 3 пустые строки с интервалом 1.5
  #v(75.5pt)

  // ---- блок «Выполнил/Руководитель»: одинарный интервал ----
  #block[
    #set par(spacing: 0pt, leading: line-1, first-line-indent: 0pt, justify: false)
    Выполнил\
    #fio-line([студент гр. #group], [#student])
    #v(12.6pt)
    Руководитель
    #v(3pt)
    #fio-line([#supervisor-role], [#supervisor])
  ]

  // в docx: 8 пустых строк 1.15 + 6 pt перед городом
  #v(154.6pt)

  #align(center)[#city – #year]
  #pagebreak()
]

// ============================================================================
// применение всех правил оформления: #show: report
// ============================================================================
#let report(body) = {
  // ------------------------- страница ---------------------------------------
  // поля: левое – 3, правое – 1, верхнее и нижнее – 2; колонтитулов нет
  set page(paper: "a4", margin: (top: 2cm, bottom: 2cm, left: 3cm, right: 1cm))

  // ------------------------- основной текст ---------------------------------
  // Times New Roman, 14 pt, абзац 1.25, междустрочный интервал 1.5,
  // выравнивание по ширине, переносы включены
  set text(lang: "ru", font: font-main, size: 14pt)
  // Края строки — метрики Times New Roman (ascender 0.891em = 12.47pt при
  // 14 pt). БЕЗ этой настройки Typst считает строку от cap-height и все
  // интервалы выходают примерно на 30% меньше, чем в Word.
  set text(top-edge: 12.47pt, bottom-edge: "descender")

  set par(
    justify: true,
    leading: line-15,
    // межабзацный интервал = интервалу строк (в Word абзацы не разделяются)
    spacing: line-15,
    // красная строка у КАЖДОГО абзаца, включая идущие после заголовков,
    // таблиц, рисунков и листингов (исключение — «где …»: см. where-par)
    first-line-indent: (amount: par-indent, all: true),
  )

  // ------------------------- подписи (общее) --------------------------------
  // «Рисунок N – Название», «Таблица N – Название», «Листинг N – Название»:
  // без красной строки, междустрочный интервал 1 (16.1 pt)
  show figure.caption: it => [
    #set par(first-line-indent: 0pt, justify: false, leading: line-1, spacing: line-1)
    #text(hyphenate: false)[#it.supplement #it.counter.display(it.numbering) – #it.body]
  ]

  // ------------------------- заголовки --------------------------------------
  // нумерация: раздел — «1», подраздел — «1.1», пункт — «1.1.1», подпункт —
  // «1.1.1.1»; Times New Roman 14 pt, по центру, переносы выключены;
  // зазор «номер ↔ текст» = 0.75 см (табулятор в docx), а не один пробел
  set heading(numbering: "1.1.1.1")
  show heading: set text(font: font-main, size: 14pt, hyphenate: false)

  // раздел: с новой страницы, КАПС, жирный; пустой строки сверху НЕТ —
  // заголовок стоит ровно на верхнем поле (в docx — pageBreakBefore)
  show heading.where(level: 1): it => {
    pagebreak(weak: true)
    block(width: 100%, above: 0pt, below: 0.62em)[
      #set par(first-line-indent: 0pt, justify: false, spacing: 0pt)
      #align(center)[
        #set text(weight: "bold")
        #if it.numbering != none [#context counter(heading).display(it.numbering)#h(0.75cm)]#upper(it.body)
      ]
    ]
  }

  // подраздел — жирный; перед ним пустая строка 1.5 + зазор строки заголовка
  show heading.where(level: 2): it => block(width: 100%, above: gap-empty-15, below: 0.62em)[
    #set par(first-line-indent: 0pt, justify: false, spacing: 0pt)
    #align(center)[
      #set text(weight: "bold")
      #if it.numbering != none [#context counter(heading).display(it.numbering)#h(0.75cm)]#it.body
    ]
  ]

  // пункт — жирный курсив
  show heading.where(level: 3): it => block(width: 100%, above: gap-empty-15, below: 0.62em)[
    #set par(first-line-indent: 0pt, justify: false, spacing: 0pt)
    #align(center)[
      #set text(weight: "bold", style: "italic")
      #if it.numbering != none [#context counter(heading).display(it.numbering)#h(0.75cm)]#it.body
    ]
  ]

  // подпункт — жирный курсив
  show heading.where(level: 4): it => block(width: 100%, above: gap-empty-15, below: 0.62em)[
    #set par(first-line-indent: 0pt, justify: false, spacing: 0pt)
    #align(center)[
      #set text(weight: "bold", style: "italic")
      #if it.numbering != none [#context counter(heading).display(it.numbering)#h(0.75cm)]#it.body
    ]
  ]

  // ------------------------- рисунки ----------------------------------------
  // «Рисунок N – Название» ПОД рисунком, по центру, интервал 1;
  // пустая строка 1.5 до рисунка и после названия; рисунок по центру
  show figure.where(kind: image): set figure(supplement: [Рисунок], numbering: "1")
  show figure.where(kind: image): it => block(above: gap-empty-15-block, below: gap-after-caption, breakable: true)[
    #set par(first-line-indent: 0pt, justify: false, leading: line-1, spacing: 0pt)
    #align(center)[#it.body]
    #align(center)[#it.caption]
  ]

  // ------------------------- таблицы ----------------------------------------
  // «Таблица N – Название» — СВЕРХУ таблицы, СЛЕВА (в docx стиль «Название
  // таблицы» без выравнивания, т.е. по левому краю — не по центру!);
  // перед таблицей пустая строка с интервалом 1, после — с интервалом 1.5
  show figure.where(kind: table): set figure(supplement: [Таблица], numbering: "1")
  show figure.where(kind: table): it => block(width: 100%, above: gap-empty-1, below: empty-line-15, breakable: true)[
    #set par(first-line-indent: 0pt, justify: false, leading: line-1, spacing: line-1)
    #align(left)[#it.caption]
    // таблицу прижимаем влево и ЯВНО сбрасываем выравнивание ячеек:
    // figure по умолчанию центрирует своё содержимое, и без align(left)
    // текст ячеек уедет по центру. Нужна таблица по центру страницы (как
    // «Таблица 1» в docx) — оберните её в align(center)[#figure(…)]
    #align(left)[#it.body]
  ]
  // границы 0.5 pt (Word «Сетка таблицы»), поля ячеек Word: слева/справа
  // 0.19 см; y = 1.5 pt компенсирует высоту строки так, чтобы строка
  // таблицы занимала 18.5 pt (интервал 1.15), как в Word
  show table: set table(stroke: 0.5pt, inset: (x: 5.4pt, y: 1.5pt))
  show table: set text(font: font-main, size: 14pt)
  // внутри таблицы интервал 1.15, без красной строки, по левому краю
  show table: set par(first-line-indent: 0pt, justify: false, leading: line-115, spacing: line-115)

  // ------------------------- листинги ---------------------------------------
  // «Листинг N – Название» — СВЕРХУ, слева, интервал 1;
  // до подписи пустая строка с интервалом 1 (в docx перед подписью листинга
  // стоит пустая строка однострочного стиля), после листинга — 1.5
  show figure.where(kind: "listing"): set figure(supplement: [Листинг], numbering: "1")
  show figure.where(kind: "listing"): it => block(width: 100%, above: gap-empty-1, below: empty-line-15, breakable: true)[
    #set par(first-line-indent: 0pt, justify: false, leading: line-1, spacing: line-1)
    #align(left)[#it.caption]
    #align(left)[#it.body]
  ]

  // ------------------------- формулы ----------------------------------------
  // нумеруются арабскими цифрами в круглых скобках у правого поля: (1), (2)…
  // Пустых строк вокруг формулы в docx НЕТ — формула стоит в общем потоке
  set math.equation(numbering: "(1)")
  show math.equation.where(block: true): set block(above: 0.4em, below: 0.62em)

  // ------------------------- списки -----------------------------------------
  // маркер — от красной строки (1.25 см), интервал между пунктами 1.5;
  // верхний уровень «1.» — с заглавной буквы, в конце точка;
  // уровень ниже «1)»/«а)» — с маленькой буквы, в конце точка с запятой;
  // тире-список — через dash-list (или обычный list)
  set enum(numbering: "1.", indent: par-indent, body-indent: 0.63cm, spacing: line-15)
  set list(indent: par-indent, body-indent: 0.5cm, spacing: line-15, marker: [–])

  body
}

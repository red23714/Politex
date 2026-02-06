#import "@preview/lilaq:0.5.0" as lq
#import "@preview/fletcher:0.5.8" as fletcher: diagram, node, edge
#import fletcher.shapes: ellipse, parallelogram, diamond, rect

https://lilaq.org/docs/quickstart#the-first-plot \
https://typst.app/universe/package/fletcher/

#lq.diagram(
  lq.plot((0, 1, 2, 3, 4), (3, 5, 4, 2, 3))
)

// === КОНФИГУРАЦИЯ МАСШТАБА ===
// Измените этот параметр для управления размером всей блок-схемы
#let scale-factor = 0.8  // 1.0 = нормальный размер, 0.5 = в 2 раза меньше, 1.5 = в 1.5 раза больше

// Производные размеры (автоматически масштабируются)
#let base-width = 3.2cm * scale-factor
#let node-spacing = (2cm * scale-factor, 1.5cm * scale-factor)
#let stroke-width = 1pt * scale-factor
#let node-inset = 8pt * scale-factor
#let font-size = 10pt * scale-factor

// === ШАБЛОН ДЛЯ БЫСТРОГО СОЗДАНИЯ БЛОК-СХЕМ ===

// Основные настройки
#let flow-node(coord, content, shape-type, name) = {
  node(coord, 
    [#align(center, text(size: font-size, content))], 
    name: name,
    shape: shape-type,
    width: base-width,
    height: auto,
    inset: node-inset
  )
}

// Предопределенные фигуры для блок-схем
#let start(coord, content, name) = flow-node(coord, content, ellipse, name)
#let input(coord, content, name) = flow-node(coord, content, parallelogram.with(angle: 30deg), name)
#let process(coord, content, name) = flow-node(coord, content, rect, name)
#let decision(coord, content, name) = flow-node(coord, content, diamond, name)
#let output(coord, content, name) = input(coord, content, name)  // такой же как input
#let end(coord, content, name) = start(coord, content, name)     // такой же как start

// Функция для создания стрелки с меткой
#let arrow(from, to, label: none, bend: 0deg) = {
  if label == none {
    edge(from, "->", to, bend: bend)
  } else {
    edge(from, "->", to, 
      label: text(size: font-size * 0.9, label),
      bend: bend
    )
  }
}

// === ПРИМЕР БЛОК-СХЕМЫ ===
#diagram(
  spacing: node-spacing,
  node-stroke: black + stroke-width,
  node-fill: white,
  label-size: font-size * 0.9,
  edge-stroke: black + (0.5pt * scale-factor),
  
  // Блок-схема вычисления максимального числа
  start((0, 0), "Начало программы", <start>),
  input((0, 1), "Ввести числа a, b, c", <input>),
  process((0, 2), "Инициализация: max = a", <init>),
  decision((0, 3), "b > max?", <cond1>),
  process((-1.8, 4), "max = b", <update1>),
  decision((0, 5), "c > max?", <cond2>),
  process((1.8, 5), "max = c", <update2>),
  output((0, 6), "Вывести значение max", <output>),
  end((0, 7), "Конец программы", <end>),
  
  // Стрелки
  arrow(<start>, <input>),
  arrow(<input>, <init>),
  arrow(<init>, <cond1>),
  arrow(<cond1>, <update1>, label: "Да"),
  arrow(<update1>, <cond2>),
  arrow(<cond1>, <cond2>, label: "Нет"),
  arrow(<cond2>, <update2>, label: "Да"),
  arrow(<update2>, <output>),
  arrow(<cond2>, <output>, label: "Нет"),
  arrow(<output>, <end>)
)

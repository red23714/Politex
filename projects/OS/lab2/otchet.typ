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
      #text(size: 16pt)[*ЛАБОРАТОРНАЯ РАБОТА № 2*] \
      \
      *«МЕХАНИЗМЫ СЕТЕВОГО ВЗАИМОДЕЙСТВИЯ»* \ 
      \
      по дисциплине «Операционные системы»
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
      #align(right)[Гавва Г. Д.]
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
Цель работы — изучить программный интерфейс сетевых сокетов, получить навыки организации взаимодействия программ при помощи протоколов Internet и разработки прикладных сетевых сервисов.

#pagebreak()

= Постановка задачи
+ Задача 1
 - Необходимо реализовать сетевую программу (tcpclient.cpp), которая считывала бы все эти сообщения из текстового файла и передавала их на удаленный сервер, прослушивающий TCP порт. Протокол взаимодействия клиента и сервера указан в вашем варианте задания. Адрес и порт сервера, а также имя входного файла указываются как аргументы командной строки при запуске клиента, например:
  tcpclient 192.168.50.7:9000 file1.txt \ где tcpclient - имя исполняемого бинарного файла клиента (получен путем компиляции tcpclient.cpp), \ 192.168.50.7 - IPv4 адрес сервера, 9000 - прослушиваемый сервером TCP-порт, file1.txt - файл с сообщениями.
  \
 - Необходимо реализовать на языке C/C++ сервер tcpserver.cpp, который можно было бы использовать вместо эмулятора (tcpserveremul.rb). Сервер должен прослушивать TCP-порт, принимать входящие подключения от клиентов, а затем - принимать сообщения, передаваемые клиентами. На каждое полученное сообщение сервер отправляет клиенту подтверждение "ok", согласно спецификации протокола. Все полученные сообщения сервер распечатывает в файл msg.txt. Каждое сообщение в msg.txt предваряется данными о клиенте: IP-адрес, двоеточие, порт клиента, пробел. \ Далее следует сообщение, которое распечатывается в таком же виде, как и было во входном файле клиента. Если от какого-либо клиента пришло сообщение с текстом "stop", то сервер, после отправки этому клиенту подтверждения "ok", закрывает все открытые соединения (отключает всех клиентов) и завершает работу. Номер прослушиваемого порта передается серверу при запуске как первый аргумент командной строки, например: tcpserver 9000 где tcpserver - имя бинарного файла программы, 9000 - номер TCP порта для прослушивания.
 \ 
+ Задача 2
 - Реализуйте программу udpclient.cpp, взаимодействующую с удаленным сервером по протоколу UDP. Программа запускается аналогично tcpclient, с указанием IP-адреса и порта удаленного сервера (но уже UDP-порта) и имени входного файла с сообщениями: \ udpclient 192.168.50.7:8700 file1.txt
 \ 
 - Реализуйте udpserver.cpp, который можно было бы использовать вместо Ruby-сервера-эмулятора.


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

#set heading(numbering: "1.")
#show heading.where(level: 1): set text(size: 16pt)

#set page(
  paper: "a4",
  margin: (
    top: 2.54cm,
    bottom: 2.54cm,
    right: 2cm,
    left: 2cm
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


#align(center)[
  Министерство образования и науки Российской Федерации \
  Санкт-Петербургский Политехнический Университет Петра Великого \
  — \
  Институт компьютерных наук и кибербезопасности
]

#v(4.5cm)

#align(center)[
  #text(size: 16pt)[*ЛАБОРАТОРНАЯ РАБОТА № 3*] \
  \
  *«Нахождение n-нного элемента последовательности»* \
  \
  по дисциплине «Структуры данных»
]

#v(3.5cm)

#box(width: 100%)[
  #box(width: 32%)[
    Выполнил \
    студент гр.5151001/40001
  ]
  #box(width: 35%)[
    #align(center)[
      \
      #symbol("<")подпись#symbol(">")
    ]
  ]
  #box(width: 29%)[
    #align(right)[Волошкевич М.А.]
  ]
]

#v(2cm)

#box(width: 100%)[
  #box(width: 32%)[
    Преподаватель / \
    ассистент
  ]
  #box(width: 35%)[
    #align(center)[
      \
      #symbol("<")подпись#symbol(">")
    ]
  ]
  #box(width: 29%)[
    #align(right)[Семьянов П.В.]
  ]
]


#v(5.79cm)

#align(center)[
  Санкт-Петербург \
  2025 г.
]

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

#set heading(numbering: none)
= Приложения
Приложение А \
main.c
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qr_encoder.h"
#include "lzw.h"

void print_usage()
{
	printf("Usage:\n");
	printf("  Compression:   ./main -c <input_file> <output_file> --password "
		   "<password>\n");
	printf("  Decompression: ./main -d <input_file> <output_file> --password "
		   "<password>\n");
}

int main(int argc, char* argv[])
{
	if (argc != 6 || strcmp(argv[4], "--password") != 0)
	{
		print_usage();
		return 1;
	}

	char mode = argv[1][1];
	char* input_file = argv[2];
	char* output_file = argv[3];
	char* password = argv[5];

	if (mode == 'c')
	{
		// Читаем входной файл
		FILE* f = fopen(input_file, "rb");
		if (!f)
		{
			perror("input");
			return 1;
		}
		fseek(f, 0, SEEK_END);
		long size = ftell(f);
		fseek(f, 0, SEEK_SET);
		char* buf = malloc(size + 1); // +1 для '\0'
		fread(buf, 1, size, f);
		buf[size] = '\0';
		fclose(f);

		// Генерируем QR PPM в памяти
		unsigned char* ppm_buf = NULL;
		size_t ppm_size = 0;
		if (!generate_qr_mem(buf, size, password, &ppm_buf, &ppm_size))
		{
			printf("QR generation failed\n");
			free(buf);
			return 1;
		}
		free(buf);

		// Сжимаем LZW в память
		unsigned char* compressed = NULL;
		size_t compressed_size =
			lzw_compress_mem(ppm_buf, ppm_size, &compressed);
		free(ppm_buf);

		// Сохраняем результат
		FILE* out = fopen(output_file, "wb");
		fwrite(compressed, 1, compressed_size, out);
		fclose(out);
		free(compressed);

		printf("Compression completed. Output: %s\n", output_file);
	}
	else if (mode == 'd')
	{
		// Читаем сжатый файл
		FILE* f = fopen(input_file, "rb");
		if (!f)
		{
			perror("input");
			return 1;
		}
		fseek(f, 0, SEEK_END);
		long size = ftell(f);
		fseek(f, 0, SEEK_SET);
		unsigned char* compressed = malloc(size);
		fread(compressed, 1, size, f);
		fclose(f);

		// Декомпрессия в память
		unsigned char* decompressed = NULL;
		size_t decompressed_size =
			lzw_decompress_mem(compressed, size, &decompressed);
		free(compressed);

		// Сохраняем декомпрессированный PPM
		FILE* out = fopen(output_file, "wb");
		fwrite(decompressed, 1, decompressed_size, out);
		fclose(out);
		free(decompressed);

		printf("Decompression completed. Output: %s\n", output_file);
	}
	else
	{
		print_usage();
		return 1;
	}
	return 0;
}
```

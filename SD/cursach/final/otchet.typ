# Отчет по оптимизациям QR-кодировщика и декодировщика

## 1. Введение

В данном отчете рассматриваются изменения, внесенные в старые версии файлов `qr_encoder.c` и `qr_decoder.c` по сравнению с новыми версиями. Основное внимание уделяется оптимизациям по использованию памяти, структуре кода и упрощению логики.

## 2. Оптимизации в `qr_encoder.c`

### 2.1 Шифрование данных

**Старый вариант:**

```c
int* sequence = malloc(len * sizeof(int));
generate_sequence(password, sequence, len);
for (size_t i = 0; i < len; i++) {
    int pos = data[i] - MIN_CHAR;
    pos = pos ^ sequence[i];
    pos = pos % RANGE;
    if (pos < 0) pos += RANGE;
    data[i] = pos + MIN_CHAR;
}
free(sequence);
```

**Новый вариант:**

```c
const size_t pass_len = strlen(password);
const unsigned char* pass = (const unsigned char*)password;
unsigned char* buf = (unsigned char*)data;
for (size_t i = 0; i + pass_len <= len; i += pass_len) {
#pragma GCC unroll 8
    for (size_t j = 0; j < pass_len; ++j) {
        unsigned char v = buf[i + j] - MIN_CHAR;
        v += pass[j];
        if (v >= RANGE) v -= RANGE;
        buf[i + j] = v + MIN_CHAR;
    }
}
for (size_t i = (len / pass_len) * pass_len; i < len; ++i) {
    unsigned char v = buf[i] - MIN_CHAR;
    v += pass[i % pass_len];
    if (v >= RANGE) v -= RANGE;
    buf[i] = v + MIN_CHAR;
}
```

**Эффект:**

* Устраняется необходимость выделять отдельный массив для последовательности, что сокращает использование памяти.
* Используется прямое арифметическое преобразование символов с циклическим применением пароля, упрощая код и уменьшая количество операций.
* Применение `#pragma GCC unroll` позволяет компилятору оптимизировать внутренние циклы.

### 2.2 Разделение строки на три части

**Старый вариант:**

```c
*part1 = malloc(part_length + remainder + 2);
*part2 = malloc(part_length + 2);
*part3 = malloc(part_length + 2);
(*part1)[0] = '$';
(*part2)[0] = '$';
(*part3)[0] = '$';
memcpy(*part1 + 1, input_string, part_length + remainder);
memcpy(*part2 + 1, input_string + part_length + remainder, part_length);
memcpy(*part3 + 1, input_string + part_length * 2 + remainder, part_length);
```

**Новый вариант:**

```c
char* a = malloc(l1 + 2);
char* b = malloc(l2 + 2);
char* c = malloc(l3 + 2);
snprintf(a, l1 + 2, "$%.*s", (int)l1, input);
snprintf(b, l2 + 2, "$%.*s", (int)l2, input + l1);
snprintf(c, l3 + 2, "$%.*s", (int)l3, input + l1 + l2);
*p1 = a; *p2 = b; *p3 = c;
```

**Эффект:**

* Код становится более компактным и безопасным, так как `snprintf` автоматически обрабатывает нуль-терминаторы.
* Уменьшается вероятность ошибок при работе с памятью.

### 2.3 Создание QR-маски

**Старый вариант:**

```c
RGB** mask = malloc(size->height * sizeof(RGB*));
for (int i = 0; i < size->height; i++) {
    mask[i] = calloc(size->width, sizeof(RGB));
}
for (int y = 0; y < qr_module_size; y++) {
    for (int x = 0; x < qr_module_size; x++) {
        if (qr->data[y * qr_module_size + x] & 1) {
            for (int sy = 0; sy < scale; sy++)
                for (int sx = 0; sx < scale; sx++)
                    mask[py][px] = (RGB){255,255,255};
        }
    }
}
```

**Новый вариант:**

```c
RGB** mask = malloc(size->height * sizeof(RGB*));
RGB* data_buf = calloc(size->height * size->width, sizeof(RGB));
for (int y = 0; y < size->height; ++y)
    mask[y] = data_buf + y * size->width;
for (int y = 0; y < modules; ++y)
    for (int x = 0; x < modules; ++x)
        if (q[y * modules + x] & 1) {
            const int base_x = pad + x * scale;
            const int base_y = pad + y * scale;
            for (int sy = 0; sy < scale; ++sy) {
                RGB* row = mask[base_y + sy] + base_x;
#pragma GCC unroll 4
                for (int sx = 0; sx < scale; ++sx)
                    row[sx] = (RGB){255, 255, 255};
            }
        }
```

**Эффект:**

* Одномерный буфер `data_buf` уменьшает количество выделений памяти и предотвращает фрагментацию.
* Структура `mask[y] = data_buf + y * width` упрощает доступ к строкам.
* Циклы с `#pragma GCC unroll` ускоряют заполнение пикселей.

### 2.4 Центрирование и наложение цвета

**Старый вариант:**

* Для каждого канала создаются отдельные массивы и отдельные циклы применяют маску к каждому каналу.

**Новый вариант:**

```c
const int off_x = (os.width - ms.width) >> 1;
const int off_y = (os.height - ms.height) >> 1;
for (int y = 0; y < ms.height; ++y) {
    RGB* m = mask[y];
    RGB* o = out[y + off_y] + off_x;
    for (int x = 0; x < ms.width; ++x) {
        if (m[x].r) {
            o[x].r |= color.r;
            o[x].g |= color.g;
            o[x].b |= color.b;
        }
    }
}
```

**Эффект:**

* Используется смещение для центрирования маски, убирается необходимость отдельной комбинации каналов.
* Меньше выделений памяти, более компактная и понятная логика наложения цвета.

## 3. Оптимизации в `qr_decoder.c`

### 3.1 Дешифрование

**Старый вариант:**

```c
int* sequence = malloc(len * sizeof(int));
generate_sequence(password, sequence, len);
for (size_t i = 0; i < len; i++) {
    int pos = data[i] - MIN_CHAR;
    pos = pos ^ sequence[i];
    pos = pos % RANGE;
    if (pos < 0) pos += RANGE;
    data[i] = pos + MIN_CHAR;
}
free(sequence);
```

**Новый вариант:**

```c
const size_t pass_len = strlen(password);
const unsigned char* pass = (const unsigned char*)password;
unsigned char* buf = (unsigned char*)data;
for (size_t i = 0; i + pass_len <= len; i += pass_len) {
#pragma GCC unroll 8
    for (size_t j = 0; j < pass_len; ++j) {
        unsigned char v = buf[i + j] - MIN_CHAR;
        if (v < pass[j]) v += RANGE;
        v -= pass[j];
        buf[i + j] = v + MIN_CHAR;
    }
}
for (size_t i = (len / pass_len) * pass_len; i < len; ++i) {
    unsigned char v = buf[i] - MIN_CHAR;
    if (v < pass[i % pass_len]) v += RANGE;
    v -= pass[i % pass_len];
    buf[i] = v + MIN_CHAR;
}
```

**Эффект:**

* Убирается дополнительный массив для последовательности, уменьшается использование памяти.
* Прямое арифметическое вычитание позволяет работать блоками данных с циклическим применением пароля.

### 3.2 Чтение PPM

**Старый вариант:**

```c
RGB** image = malloc(*height * sizeof(RGB*));
for (int i = 0; i < *height; i++) {
    image[i] = malloc(*width * sizeof(RGB));
    fread(image[i], sizeof(RGB), *width, fp);
}
```

**Новый вариант:**

```c
RGB* buf = malloc(sizeof(RGB) * (*width) * (*height));
fread(buf, sizeof(RGB), (*width) * (*height), fp);
```

**Эффект:**

* Одномерный массив уменьшает фрагментацию памяти и упрощает доступ к пикселям.
* Можно получать указатели на строки через `buf + y * width` без дополнительных выделений.

### 3.3 Извлечение цветовых каналов

**Старый вариант:**

* Для каждой строки и пикселя отдельно вычисляется значение канала и инвертируется.

**Новый вариант:**

```c
switch (channel) {
case 'r':
    for (int i = 0; i < n; i++) dst[i] = 255 - src[i].r;
    break;
case 'g':
    for (int i = 0; i < n; i++) dst[i] = 255 - src[i].g;
    break;
case 'b':
    for (int i = 0; i < n; i++) dst[i] = 255 - src[i].b;
    break;
default:
    memset(dst, 0, n);
}
```

**Эффект:**

* Прямой доступ к одномерному буферу позволяет копировать и инвертировать канал за один цикл.
* Упрощается код и уменьшается количество циклов и malloc.

## 4. Заключение

Оптимизации заключаются в:

* Устранении лишних массивов для последовательностей и изображений.
* Использовании одномерных буферов вместо многократных `malloc`, что уменьшает фрагментацию.
* Применении циклических преобразований данных с помощью пароля.
* Использовании директив `#pragma GCC unroll` для ускорения внутренних циклов.

Эти изменения делают код более компактным, удобным для поддержки и эффективным по использованию памяти.

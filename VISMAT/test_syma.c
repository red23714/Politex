
#include <symengine/cwrapper.h>
#include <stdio.h>

int main() {
    // Создаём структуры (объекты, не массивы!)
    basic x;
    basic f;
    basic two;
    basic d;

    // Инициализация
    basic_init(x);
    basic_init(f);
    basic_init(two);
    basic_init(d);

    // x = "x"
    symbol_set(x, "x");

    // two = 2.0
    real_double_set_d(two, 2.0);

    // f = x^2
    pow_basic(f, x, two);

    // d = diff(f, x)
    diff_basic(d, f, x);

    // Выводим результат
    char* s = basic_str(d);
    printf("diff(x^2) = %s\n", s);
    basic_str_free(s);

    // Освобождение памяти
    basic_free(x);
    basic_free(f);
    basic_free(two);
    basic_free(d);

    return 0;
}


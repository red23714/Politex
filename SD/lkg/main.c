#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>
#include "hash.h"
#include <math.h>

#define ull unsigned long long

ull a = 92233720368547765ULL;
ull b = 92233720368547764ULL;
ull c = 3737373737ULL;
ull M = 18446744073709551615ULL;

ull randim(ull x) {
    return (x * a + c);
}

void power() {
    int s = 1;
    ull b1 = b = 92233720368547764ULL;
    for (int i = 0; i < 128; i++) {
        b = b * b1;
        s++;
        if (b == 0) {
            printf("мощность = %d\n\n", s);
            return;
        }
    }
}

void hi_kvadrat(ull x) {

    double v21[8] = { 8.260, 10.85, 12.45, 19.34, 28.412, 31.41, 37.57 };
    int bed = 0;
    int good = 0;
    int neg = 0;

    ull r = 0;
    printf("Введите правую границу: ");
    scanf("%llu", &r);

    ull n = 0;
    printf("Количество измерений: ");
    scanf("%llu", &n);

    ull k = 21;
    int p = 100;
    while (p--) {
        ull Y_i[21] = { 0 };

        double np_i = n / k;
        ull t = n;
        ull chislo = 0;

        while (t--) {
            x = randim(x);
            chislo = x % r;
            
            for (int i = 0; i < 21; i++) {
                if (i * (r / 21) <= chislo && chislo < (i + 1) * (r / 21)) {
                    Y_i[i]++;
                    break;
                }
            }
        }
       
        double hi = 0;
        for (int i = 0; i < k; i++) {
            double v0 = (double)Y_i[i] - np_i;
            hi +=  ((v0 * v0) / np_i);
        }
        if (v21[0] <= hi && hi <= v21[1]) { printf("Результат hi-квадрата(%.3f) плохой\n", hi); neg++; }
        else if (v21[1] <= hi && hi <= v21[2]) { printf("Результат hi-квадрата(%.3f) подозрителен\n", hi); bed++; }
        else if (v21[2] <= hi && hi <= v21[5]) {printf("Результат hi-квадрата(%.3f) отличный\n", hi); good++;}
        else if (v21[5] <= hi && hi <= v21[6]){ printf("Результат hi-квадрата(%.3f) подозрителен\n", hi); bed++;}
        else if (v21[6] <= hi && hi <= v21[7]){ printf("Результат hi-квадрата(%.3f) плохой\n", hi); neg++;}
    }
    printf("Отличных hi = %d, подозрительныx hi = %d , плохих hi = %d, ужасные hi = %d\n\n", good, bed, neg, 100-good-bed-neg);
}

void birthday_paradox(ull initial_seed) {
    ull num_experiments = 0;
    ull num_generations_per_experiment = 0;

    printf("Введите количество экспериментов: ");
    scanf("%llu", &num_experiments);

    printf("Введите количество людей в каждом эксперименте (n): ");
    scanf("%llu", &num_generations_per_experiment);

    if (num_experiments == 0 || num_generations_per_experiment == 0) {
        printf("Количество экспериментов и людей должно быть больше 0.\n");
        return;
    }

    // Размер хеш-таблицы - 365, так как у нас дни года (1-365)
    size_t hashtable_size = 365;
    int experiments_with_collision = 0;

    printf("Проведение %llu экспериментов...\n", num_experiments);

    ull current_seed = initial_seed;

    for (ull i = 0; i < num_experiments; ++i) {
        HashTable* ht = create_hashtable(hashtable_size);
        if (ht == NULL) {
            perror("Ошибка создания хеш-таблицы в эксперименте");
            return;
        }

        bool collision_in_experiment = false;
        ull current_x = current_seed;

        for (ull j = 0; j < num_generations_per_experiment; ++j) {
            current_x = randim(current_x);
            // Ограничиваем диапазон до 1-365 (дни года)
            ull day_of_year = (current_x % 365) + 1;
            
            if (insert_hashtable(ht, day_of_year)) {
                collision_in_experiment = true;
                break;
            }
        }

        free_hashtable(ht);

        if (collision_in_experiment) {
            experiments_with_collision++;
        }

        current_seed = randim(current_seed);
    }

    printf("\nРезультаты:\n");
    printf("Количество экспериментов: %llu\n", num_experiments);
    printf("Количество людей в каждом эксперименте (n): %llu\n", n);
    printf("Количество экспериментов с совпадением дней рождения: %d\n", experiments_with_collision);
    printf("Наблюдаемая вероятность совпадения: %.2f%%\n", observed_prob);
    printf("Теоретическая вероятность совпадения (парадокс дней рождений): %.2f%%\n", expected_prob);

    // Интерпретация результатов
    printf("\nИнтерпретация:\n");
    printf("Если наблюдаемая вероятность близка к теоретической, генератор работает хорошо.\n");
    printf("При n=23 теоретическая вероятность должна быть около 50.7%%\n");
    printf("При n=50 теоретическая вероятность должна быть около 97%%\n");
}


void razbrosik(ull x) {
    ull r = 0;
    printf("Введите правую границу: ");
    scanf("%llu", &r);

    ull n = 0;
    printf("Количество измерений: ");
    scanf("%llu", &n);

    ull* mass = (ull*)calloc(r, sizeof(ull));

    int y = n;
    while (y--) {
        mass[x % r]++;
        x = randim(x);
    }
    for (int i = 0; i < r; i++) {
        printf("число %llu встретилось %llu раз\n", i, mass[i]);
    }
    free(mass);
}

void period(ull x) {
    ull x1, x2, x3;
    x1 = randim(x);
    x2 = randim(x1);
    x3 = randim(x2);
    x = x3;
    ull n1, n2, n3;
    ull h = 0;
    while (1) {
        n1 = randim(x);
        x = n1;
        n2 = randim(n1);
        n3 = randim(n2);
        if (h % 1000000 == 0)printf("%llu   ", h);
        if (n1 == x1 && n2 == x2 && n3 == x3) {
            break;
        }
        h++;
    }
}

int main(){
    setlocale(LC_ALL, "Rus");
    int g = 0;
    ull seed = 0;
    ull x = 0;
    while (1) {
        switch (g)
        {
        default:
            printf("Выберите критерий: 1)Разброс   2)Критерий хи-квадрат   3)Критерий по парадоксу дней рождений   4)Мощность    5)Период \n");
            scanf("%d", &g);
            break;
        case 1:
            seed = time(NULL);
            x = randim(seed);
            razbrosik(x);
            g = 0;
            break;
        case 2:
             seed = time(NULL);
            x = randim(seed);
            hi_kvadrat(x);
            g = 0;
            break;
        case 3:
            seed = time(NULL);
             x = randim(seed);
            birthday_paradox(x);
            g = 0;
            break;
        case 4:
             seed = time(NULL);
             x = randim(seed);
            power();
            g = 0;
            break;
        case 5:
            seed = time(NULL);
             x = randim(seed);
            period(x);
            g = 0;
            break;
    }
    }

    return 0;
}

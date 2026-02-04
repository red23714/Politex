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
    double v21[6] = { 2.558, 3.94, 4.865, 15.99, 18.31, 23.31 };
    
    int bad = 0;
    int good = 0;
    int neg = 0;

    ull r = 0;
    printf("Введите диапозон: ");
    scanf("%llu", &r);

    ull n = 0;
    printf("Количество чисел: ");
    scanf("%llu", &n);

    ull k = 11;
    for (int i = 0; i < 100; i++)  
    {
        ull Y_i[11] = { 0 };

        double np_i = (double)n / k;
        ull t = n;
        ull chislo = 0;

        for (int i = 0; i < n; i++)     
        {
            x = randim(x);
            chislo = x % r;
            
            for (int i = 0; i < 11; i++) 
            {
                if (i * (r / 11) <= chislo && chislo < (i + 1) * (r / 11)) 
                {
                    Y_i[i]++;
                    break;
                }
            }
        }
       
        double hi = 0;
        for (int i = 0; i < k; i++) 
        {
            double v0 = (double)Y_i[i] - np_i;
            hi +=  ((v0 * v0) / np_i);
        }

        printf("Тест %d: hi-квадрат = %.3f, ", i + 1, hi);
        if (hi <= v21[0] || hi >= v21[5]) {
            printf("Результат плохой\n");
            neg++;
        }
        else if (hi <= v21[1] || hi >= v21[4]) {
            printf("Результат подозрителен\n");
            bad++;
        }
        else if (hi <= v21[2] || hi >= v21[3]) {
            printf("Результат почти подозрительный\n");
            bad++;
        }
        else {
            printf("Результат хороший\n");
            good++;
        }
    }
    printf("Отличных hi = %d, подозрительныx hi = %d , плохих hi = %d, ужасные hi = %d\n\n", good, bad, neg, 100 - good - bad - neg);
}

void birthday_paradox(ull initial_seed) {
    ull num_calculations = 0;
    ull num_people = 0;

    ull counts_of_correct = 0;
    int fringe_diff = 5;

    printf("Введите количество расчетов вероятности: ");
    scanf("%llu", &num_calculations);

    printf("Введите количество людей в эксперименте (n): ");
    scanf("%llu", &num_people);

    if (num_calculations == 0 || num_people == 0) {
        printf("Количество расчетов и людей должно быть больше 0.\n");
        return;
    }

    // Вычисляем теоретическую вероятность
    double theoretical_prob = 1.0;
    for (ull i = 0; i < num_people; ++i) {
        theoretical_prob *= (365.0 - i) / 365.0;
    }
    theoretical_prob = (1.0 - theoretical_prob) * 100.0;

    printf("\nТеоретическая вероятность совпадения для %llu людей: %.2f%%\n\n", 
           num_people, theoretical_prob);

    for (ull calc = 0; calc < num_calculations; ++calc) {
        ull collisions_in_100 = 0;
        
        // Генерируем уникальный seed для каждого расчета
        ull current_calc_seed = initial_seed + calc;
        ull experiment_seed = current_calc_seed;

        printf("Расчет %llu \n", calc + 1);

        // 100 экспериментов с текущим seed
        for (ull exp = 0; exp < 10000; ++exp) {
            HashTable* ht = create_hashtable(365);
            if (ht == NULL) {
                perror("Ошибка создания хеш-таблицы");
                return;
            }

            bool has_collision = false;
            ull current_x = experiment_seed;

            for (ull person = 0; person < num_people; ++person) {
                current_x = randim(current_x);
                ull birthday = (current_x % 365) + 1;
                
                if (insert_hashtable(ht, birthday)) {
                    has_collision = true;
                    break;
                }
            }

            free_hashtable(ht);

            if (has_collision) {
                collisions_in_100++;
            }

            // Обновляем seed для следующего эксперимента
            experiment_seed = randim(experiment_seed);
        }

        double experimental_prob = (double)collisions_in_100 / 10000 * 100.0;
        double deviation = experimental_prob - theoretical_prob;

        printf("  Совпадений: %llu/10000\n", collisions_in_100);
        printf("  Вероятность: %.2f%% (отклонение: %+.2f%%)\n", 
               experimental_prob, deviation);
        printf("----------------------------------------\n");

        if(fabs(deviation) < fringe_diff) counts_of_correct++;
    }
    
    printf("Колличество тестов не превышающих границу в %d%%: %llu \n", fringe_diff, counts_of_correct);
    printf("Процуент правильных тестов: %.2f%% \n", (double)counts_of_correct / num_calculations * 100.0);
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

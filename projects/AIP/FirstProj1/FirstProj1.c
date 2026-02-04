#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
int prov_prost(int n) {
    if (n < 2) return 0;
    for (int i = 2; i * i < n; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int sum_chisl(int n) {
    int sum = 0;
    while (n > 0) {
        sum += n % 10;
        n /= 10;
    }
    return sum;
}

int prost_chisl_sum(int mass[], int sume) {
    int i = 0;
    for (int k = 10000; k < 99999; k++) {
        if (prov_prost(k) && sum_chisl(k) == sume) {
            mass[i] = k;
            i++;
        }
    }
    return i;
}

int prov_st(int n, int sume) {
    if (sum_chisl(n) > sume) return 0;
    else return sum_chisl(n);

}

void matrix(int sume, int arr[5][5], int k, int str, int mass[]) {
    if (str == 5) {
        for (int j = 0; j < 5; j++) {
            /*тут проверка столбцов*/
            int n = 0;
            for (int i = 0; i < 5; i++) {
                n = n * 10 + arr[i][j];
            }
            if (prov_prost(n) == 0 || sum_chisl(n) != sume) return; /*если не удовлетворяет, то возвращаем пустой вариант*/
        }

        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                printf("%d", arr[i][j]);
            }
            printf("\n");
        }

        int d1 = 0, d2 = 0;
        for (int i = 0; i < 5; i++) {
            d1 = d1 * 10 + arr[i][i];
            d2 = d2 * 10 + arr[i][4 - i];
        }
        if (prov_prost(d1) == 0 || sum_chisl(d1) != sume) return;
        if (prov_prost(d2) == 0 || sum_chisl(d2) != sume) return;

        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                printf("%d", arr[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }
    else {
        if (str == 0) {
            for (int i = 0; i < k; i++) {
                if (mass[i] > arr[0][0] * 10000 + 1111) {
                    int n = mass[i];
                    for (int j = 4; j >= 0; j--) {
                        arr[str][j] = n % 10;
                        n /= 10;
                        //if (prov_st(arr[str][j], sume) == 0) return;
                        //if ((sume - arr[str][j]) < 7) return; /*самое маленькое простое число 10007, сумма его цифр 8*/
                    }
                    matrix(sume, arr, k, str + 1, mass);
                }
            }
        }
        else {
            for (int i = 0; i < k; i++) {
                int n = mass[i];
                for (int j = 4; j >= 0; j--) {
                    arr[str][j] = n % 10;
                    n /= 10;
                    /*int st = 0;
                    for (int l = 0; l <= str; l++) {
                      st = st * 10 + arr[l][j];
                      if (prov_st(st, sume) == 0) return;
                    }*/
                    //if (sume - arr[str][j] < 5 - str) return;
                }
                matrix(sume, arr, k, str + 1, mass);
            }
        }
    }
    /*else {
      if (str == 0) {
        for (int tk = arr[0][0] * 10000 + 1111; tk < (arr[0][0] + 1) * 10000; tk++) {
          if (prov_prost(tk) && sum_chisl(tk) == sume) {
            int n = tk;
            for (int j = 4; j >= 0; j--) {
              arr[0][j] = n % 10;
              n /= 10;
            }
            matrix(sume, arr, 0, str + 1);
          }
        }
      }
      else {
        for (int tk = 10000; tk <= 99999; tk++) {
          if (prov_prost(tk) && sum_chisl(tk) == sume) {
            int n = tk;
            for (int j = 4; j >= 0; j--) {
              arr[str][j] = n % 10;
              n /= 10;
            }
            matrix(sume, arr, 0, str + 1);
          }

        }
      }
    }*/
}

int main()
{
    int sume, vu;
    int k;
    scanf("%d %d", &sume, &vu);

    int arr[5][5] = { 0 }, mass[90000]; /*создали матрицу и обнулили ее*/
    arr[0][0] = vu;/*поставили первый элемент на место*/
    k = prost_chisl_sum(mass, sume);/* сздали массив из чисел с одинаковой суммой, чтобы сократить перебор*/
    /*for (int i = 0; i < k; i++) {
      printf("%d\n", mass[i]);
    }*/
    matrix(sume, arr, k, 0, mass);
    return 0;
}
// C program to implement Quick Sort Algorithm
#include <stdio.h>

void qsortRecursive(int *mas, int size) 
{
    //Указатели в начало и в конец массива
    int i = 0;
    int j = size - 1;

    //Центральный элемент массива
    int mid = mas[size / 2];

    //Делим массив
    do {
        //Пробегаем элементы, ищем те, которые нужно перекинуть в другую часть
        //В левой части массива пропускаем(оставляем на месте) элементы, которые меньше центрального
        while(mas[i] < mid) 
        {
            i++;
        }
        //В правой части пропускаем элементы, которые больше центрального
        while(mas[j] > mid) 
        {
            j--;
        }

        //Меняем элементы местами
        if (i <= j) {
            int tmp = mas[i];
            mas[i] = mas[j];
            mas[j] = tmp;

            i++;
            j--;
        }
    } while (i <= j);


    //Рекурсивные вызовы, если осталось, что сортировать
    if(j > 0) {
        //"Левый кусок"
        qsortRecursive(mas, j + 1);
    }
    if (i < size) {
        //"Првый кусок"
        qsortRecursive(&mas[i], size - i);
    }
}

int main() {
  
    int arr[] = { 4, 2, 5, 3, 1 };
    int n = sizeof(arr) / sizeof(arr[0]);

    qsortRecursive(arr, n);

    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);

    return 0;
}
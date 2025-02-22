#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>

int isSquare(unsigned long long num) {
    unsigned long long root = (unsigned long long)(sqrt((double)num) + 0.5);
    return root * root == num;
}

int getMaxIndex(unsigned long long numbers[]) {
    unsigned long long largest = 0;
    int maxIndex = -1;

    for (int i = 0; i < 20; i++) {
        if (numbers[i] > largest) {
            largest = numbers[i];
            maxIndex = i;
        }
    }
    return maxIndex;
}

int main() {
    int totalNumbers;
    unsigned long long inputArray[20] = { 0 };
    unsigned long long squareArray[20] = { 0 };

    scanf("%d", &totalNumbers);

    int firstZeroIndex = -1;

    for (int i = 0; i < totalNumbers; i++) {
        scanf("%llu", &inputArray[i]);

        if (inputArray[i] == 0 && firstZeroIndex == -1) {
            firstZeroIndex = i;
        }

        if (isSquare(inputArray[i]) && inputArray[i] != 0) {
            squareArray[i] = inputArray[i];
        }
    }

    int maxSquareIndex = getMaxIndex(squareArray);

    if (maxSquareIndex != -1 && squareArray[maxSquareIndex] > 0) {
        printf("%d", maxSquareIndex);
    } else if (firstZeroIndex != -1) {
        printf("%d", firstZeroIndex);
    } else {
        printf("-1");
    }

    return 0;
}
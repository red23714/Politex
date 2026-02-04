//#define _CRT_SECURE_NO_WARNINGS
//
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
//
//void multiply(char* num1, char* num2, char* result) {
//    int len1 = strlen(num1);
//    int len2 = strlen(num2);
//    int* res = (int*)calloc(len1 + len2, sizeof(int));
//
//    for (int i = len1 - 1; i >= 0; i--) {
//        for (int j = len2 - 1; j >= 0; j--) {
//            int mul = (num1[i] - '0') * (num2[j] - '0');
//            int sum = mul + res[i + j + 1];
//
//            res[i + j + 1] = sum % 10;
//            res[i + j] += sum / 10;
//        }
//    }
//
//    int index = 0;
//    while (index < len1 + len2 && res[index] == 0) {
//        index++;
//    }
//
//    if (index == len1 + len2) {
//        result[0] = '0';
//        result[1] = '\0';
//    } else {
//        int k = 0;
//        while (index < len1 + len2) {
//            result[k] = res[index] + '0';
//            k++;
//            index++;
//        }
//        result[k] = '\0';
//    }
//
//    free(res);
//}
//
//int main() {
//    char num1[2501], num2[2501], result[5002]; 
//
//    if (scanf("%s %s", num1, num2) != 2) return 0;
//
//    multiply(num1, num2, result);
//    printf("%s", result);
//
//    return 0;
//}
//

//#define _CRT_SECURE_NO_WARNINGS
//
//#include <stdio.h>
//
//void printPartition(int arr[], int idx) {
//    if (idx == 1) return;
//    for (int i = 0; i < idx; i++) {
//        printf("%d", arr[idx - i - 1]);
//        if (i < idx - 1)
//            printf("+");
//    }
//    printf("\n");
//}
//
//void partition(int n, int max, int arr[], int idx) {
//    for (int i = 1; i <= max; i++) {
//        if (i <= n) {
//            arr[idx] = i;
//            if (n - i == 0) {
//                printPartition(arr, idx + 1);
//            }
//            else {
//                partition(n - i, i, arr, idx + 1);
//            }
//        }
//    }
//}
//
//int main() {
//    int N;
//    scanf("%d", &N);
//
//    int arr[30];
//    partition(N, N, arr, 0);
//
//    return 0;
//}


"///'/////" / '"///"////' / '/////
/ "///"//"/*///'/'//*////*///
"//"///'"//'////\"'
'/////"//'////'"/////\
'///"//"'/'//'////\
////*"//'/////"///////
/*//'/"//"//'/'/////'''
/'*"///'////"/*/"//"//
'///\"/////"/"////"'/////\
"/"/"///*////"'/""//\
"////'//'///////"////'
////'//////"'//"/////"/*
'///"/////"/"/////' / "/'////
"/////"//'/////\
'/""////"///"////'//
"////"//'////\
'//'///////"////'
////'//'///////'/////\
/'*///"/"///'//\
"'///"////'/\
"///*/'///"/////
"/"/////"/'//"/\
"'/""/""'///////\
////'"////\
"/"/"'/'///"////
'/'///"////'/////\
"//"//"/"////\
"////'"//////'
////"//'/////"////
/*///"///"////\
'////"//"/////




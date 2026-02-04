#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <time.h>

#define SIZE 6

int template[SIZE][SIZE];

typedef struct Vector2
{
    int x;
    int y;
}Vector2;

typedef struct Desk
{
    int values[SIZE][SIZE];
}Desk;

void generateValues(int [][SIZE]);
int placeHorse(int, int, int[][SIZE]);
int placeHorseRecursive(int, Desk, int);
void printDesk(int[][SIZE]);
int wherePlace(Vector2*, int[][SIZE]);
int isDeskFull(int[][SIZE]);
void clearDesk(int[][SIZE], int);
Vector2 getMinValue(int[][SIZE]);
Desk copyDesk(int [][SIZE]);

int main()
{
    Desk desk;

    generateValues(template);

    desk = copyDesk(template);

    int minValue = placeHorseRecursive(0, desk, 65);

    printf("%d\n", minValue);

    printf("time = %ld ms \n", clock()); 
    
    return 0;
}

int placeHorse(int x, int y, int desk_in[][SIZE])
{
    if (desk_in[y][x] != 0)
    {
        desk_in[y][x] = 0;

        int moves[8][2] = {
            {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
            {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};

        for (int i = 0; i < 8; i++)
        {
            int nx = x + moves[i][0];
            int ny = y + moves[i][1];
            if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE && desk_in[ny][nx] != 0)
                desk_in[ny][nx] = -1;
        }
        return 1;
    }
    return 0;
}

int wherePlace(Vector2* moves, int desk[][SIZE])
{
    int maxValue = -2;
    int count = 0;

    int virtual_desk[SIZE][SIZE];
    clearDesk(virtual_desk, 1);

    // Получаем клетку с минимальным значением для расстановки
    Vector2 min_coord = getMinValue(desk);
    placeHorse(min_coord.x, min_coord.y, virtual_desk); // Установить виртуальные ходы

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (virtual_desk[i][j] < 0 && desk[i][j] != 0 && template[i][j] > maxValue)
            {
                maxValue = template[i][j];
            }
        }
    }

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (virtual_desk[i][j] < 0 && desk[i][j] != 0 && template[i][j] == maxValue)
            {
                moves[count].y = i;
                moves[count].x = j;
                count++;
            }
        }
    }

    if (count == 0)
    {
        moves[0].y = min_coord.y;
        moves[0].x = min_coord.x;
        count++;
    }
    

    return count;
}

void clearDesk(int desk[][SIZE], int value)
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            desk[i][j] = value;
        }
    }
}

Vector2 getMinValue(int desk[][SIZE])
{
    Vector2 coord;
    int min = 100;
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (desk[i][j] < min && desk[i][j] != -1 && desk[i][j] != 0)
            {
                coord.y = i;
                coord.x = j;
                min = desk[i][j];
            }
        }
    }

    return coord;
}

int placeHorseRecursive(int count, Desk desk, int bestCount)
{
    if (count > bestCount)
    {
        return 65;
    }
    
    // Если доска заполнена, возвращаем количество расставленных коней
    if (isDeskFull(desk.values)) 
    {
        printDesk(desk.values); // Можно сохранить вывод
        printf("%d\n", count);
        
        return count; // Возврат текущего количества коней
    }

    Vector2 possible_moves[8];
    int moves_count = wherePlace(possible_moves, desk.values);

    // Проводим перебор всех возможных ходов
    for (int i = 0; i < moves_count; i++)
    {
        Desk copy = copyDesk(desk.values); // Копируем текущее состояние
        placeHorse(possible_moves[i].x, possible_moves[i].y, copy.values); // Пытаемся установить коня
        int result = placeHorseRecursive(count + 1, copy, bestCount); // Рекурсивный вызов
        
        // Сравниваем для нахождения минимального количества коней
        if (result < bestCount)
        {
            bestCount = result;
        }
    }
    
    return bestCount; // Возвращаем лучший результат
}

int isDeskFull(int desk[][SIZE])
{
    int count = 0;
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (desk[i][j] > 0) return 0;
        }
    }
    return 1;
}

void printDesk(int desk[][SIZE])
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
            printf("%2d ", desk[i][j]);
        printf("\n");
    }
    printf("\n");
}

Desk copyDesk(int original[][SIZE])
{
    Desk copy;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            copy.values[i][j] = original[i][j];
    return copy;
}

// Проверка на допустимость хода
int is_valid(int x, int y, int n) {
    return x >= 0 && x < n && y >= 0 && y < n;
}

// Функция для создания графа ходов коня
void generateValues(int graph[][SIZE]) {
    // Смещения по координатам для хода коня
    int dx[] = {2, 2, 1, -1, -2, -2, -1, 1};
    int dy[] = {1, -1, -2, -2, -1, 1, 2, 2};

    // Инициализация массива графа нулями
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            graph[i][j] = 0;
        }
    }

    // Подсчёт количества ходов в каждую клетку
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            for (int k = 0; k < 8; k++) {
                int nx = i + dx[k];
                int ny = j + dy[k];
                if (is_valid(nx, ny, SIZE)) {
                    graph[nx][ny]++;
                }
            }
        }
    }
}

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <windows.h>
#include <cuda_runtime.h>
#include <time.h>

#define THREADS 256
#define MAX_TEXT 1048576  // 1 MB максимум

__constant__ uint8_t d_sbox[256];
__constant__ uint8_t d_inv_sbox[256];
__constant__ uint8_t d_key[256];

// S-box и обратный S-box
static const uint8_t sbox[256] = {
    99,124,119,123,242,107,111,197,48,1,103,43,254,215,171,118,
    202,130,201,125,250,89,71,240,173,212,162,175,156,164,114,192,
    183,253,147,38,54,63,247,204,52,165,229,241,113,216,49,21,
    4,199,35,195,24,150,5,154,7,18,128,226,235,39,178,117,
    9,131,44,26,27,110,90,160,82,59,214,179,41,227,47,132,
    83,209,0,237,32,252,177,91,106,203,190,57,74,76,88,207,
    208,239,170,251,67,77,51,133,69,249,2,127,80,60,159,168,
    81,163,64,143,146,157,56,245,188,182,218,33,16,255,243,210,
    205,12,19,236,95,151,68,23,196,167,126,61,100,93,25,115,
    96,129,79,220,34,42,144,136,70,238,184,20,222,94,11,219,
    224,50,58,10,73,6,36,92,194,211,172,98,145,149,228,121,
    231,200,55,109,141,213,78,169,108,86,244,234,101,122,174,8,
    186,120,37,46,28,166,180,198,232,221,116,31,75,189,139,138,
    112,62,181,102,72,3,246,14,97,53,87,185,134,193,29,158,
    225,248,152,17,105,217,142,148,155,30,135,233,206,85,40,223,
    140,161,137,13,191,230,66,104,65,153,45,15,176,84,187,22
};

static const uint8_t inv_sbox[256] = {
    82,9,106,213,48,54,165,56,191,64,163,158,129,243,215,251,
    124,227,57,130,155,47,255,135,52,142,67,68,196,222,233,203,
    84,123,148,50,166,194,35,61,238,76,149,11,66,250,195,78,
    8,46,161,102,40,217,36,178,118,91,162,73,109,139,209,37,
    114,248,246,100,134,104,152,22,212,164,92,204,93,101,182,146,
    108,112,72,80,253,237,185,218,94,21,70,87,167,141,157,132,
    144,216,171,0,140,188,211,10,247,228,88,5,184,179,69,6,
    208,44,30,143,202,63,15,2,193,175,189,3,1,19,138,107,
    58,145,17,65,79,103,220,234,151,242,207,206,240,180,230,115,
    150,172,116,34,231,173,53,133,226,249,55,232,28,117,223,110,
    71,241,26,113,29,41,197,137,111,183,98,14,170,24,190,27,
    252,86,62,75,198,210,121,32,154,219,192,254,120,205,90,244,
    31,221,168,51,136,7,199,49,177,18,16,89,39,128,236,95,
    96,81,127,169,25,181,74,13,45,229,122,159,147,201,156,239,
    160,224,59,77,174,42,245,176,200,235,187,60,131,83,153,97,
    23,43,4,126,186,119,214,38,225,105,20,99,85,33,12,125
};

/* ================= GPU Helpers ================= */
__device__ void shift_left(uint8_t* data, int start, int block_size) {
    uint8_t t = data[start];
    for (int i = 0; i < block_size - 1; i++)
        data[start + i] = data[start + i + 1];
    data[start + block_size - 1] = t;
}

__device__ void shift_right(uint8_t* data, int start, int block_size) {
    uint8_t t = data[start + block_size - 1];
    for (int i = block_size - 1; i > 0; i--)
        data[start + i] = data[start + i - 1];
    data[start] = t;
}

/* ================= GPU Kernels ================= */
__global__ void encrypt_kernel(uint8_t* data, size_t blocks, int block_size, int rounds) {
    size_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= blocks) return;

    size_t start = idx * block_size;

    for (int r = 0; r < rounds; r++) {
        // XOR с ключом
        for (int i = 0; i < block_size; i++)
            data[start + i] ^= d_key[i];

        // S-box замена
        for (int i = 0; i < block_size; i++)
            data[start + i] = d_sbox[data[start + i]];

        // Циклический сдвиг влево
        shift_left(data, start, block_size);
    }
}

__global__ void decrypt_kernel(uint8_t* data, size_t blocks, int block_size, int rounds) {
    size_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= blocks) return;

    size_t start = idx * block_size;

    for (int r = 0; r < rounds; r++) {
        // Циклический сдвиг вправо (обратный сдвигу влево)
        shift_right(data, start, block_size);

        // Обратная S-box замена
        for (int i = 0; i < block_size; i++)
            data[start + i] = d_inv_sbox[data[start + i]];

        // XOR с ключом (такой же как при шифровании)
        for (int i = 0; i < block_size; i++)
            data[start + i] ^= d_key[i];
    }
}

/* ================= CPU ================= */
void cpu_encrypt(uint8_t* data, size_t blocks, const uint8_t* key, int block_size, int rounds) {
    for (size_t b = 0; b < blocks; b++) {
        size_t start = b * block_size;
        for (int r = 0; r < rounds; r++) {
            for (int i = 0; i < block_size; i++)
                data[start + i] ^= key[i];
            for (int i = 0; i < block_size; i++)
                data[start + i] = sbox[data[start + i]];

            uint8_t t = data[start];
            for (int i = 0; i < block_size - 1; i++)
                data[start + i] = data[start + i + 1];
            data[start + block_size - 1] = t;
        }
    }
}

void cpu_decrypt(uint8_t* data, size_t blocks, const uint8_t* key, int block_size, int rounds) {
    for (size_t b = 0; b < blocks; b++) {
        size_t start = b * block_size;
        for (int r = 0; r < rounds; r++) {
            uint8_t t = data[start + block_size - 1];
            for (int i = block_size - 1; i > 0; i--)
                data[start + i] = data[start + i - 1];
            data[start] = t;

            for (int i = 0; i < block_size; i++)
                data[start + i] = inv_sbox[data[start + i]];
            for (int i = 0; i < block_size; i++)
                data[start + i] ^= key[i];
        }
    }
}

/* ================= Main ================= */
int main() {
    // Устанавливаем русскую локаль
    system("chcp 1251 > nul");
    setlocale(LC_ALL, "Russian");

    // Для точного измерения времени
    LARGE_INTEGER frequency, start_cpu, end_cpu;
    QueryPerformanceFrequency(&frequency);

    int block_size, rounds;

    // Динамическое выделение памяти вместо стека
    char* text = (char*)malloc(MAX_TEXT);
    if (!text) {
        printf("Ошибка выделения памяти!\n");
        return 1;
    }

    printf("Введите текст для шифрования (макс %d символов):\n", MAX_TEXT - 1);
    fgets(text, MAX_TEXT, stdin);
    size_t text_len = strlen(text);
    if (text_len > 0 && text[text_len - 1] == '\n') {
        text[text_len - 1] = 0;
        text_len--;
    }

    printf("Введите размер блока в байтах: ");
    scanf("%d", &block_size);
    printf("Введите количество раундов: ");
    scanf("%d", &rounds);

    // Убедимся, что размер блока не превышает 256
    if (block_size > 256 || block_size <= 0) {
        printf("Размер блока должен быть в диапазоне 1-256 байт\n");
        free(text);
        return 1;
    }

    size_t padded_len = ((text_len + block_size - 1) / block_size) * block_size;
    uint8_t* h_data = (uint8_t*)calloc(padded_len, 1);
    if (!h_data) {
        printf("Ошибка выделения памяти!\n");
        free(text);
        return 1;
    }
    memcpy(h_data, text, text_len);

    // Генерация ключа (повторяющийся паттерн)
    uint8_t key[256];
    for (int i = 0; i < 256; i++)
        key[i] = i * 7;

    size_t blocks = padded_len / block_size;
    printf("\nДлина текста: %zu байт, Паддинг до: %zu байт, Блоков: %zu\n",
        text_len, padded_len, blocks);

    // CPU - шифрование
    uint8_t* cpu_buf = (uint8_t*)malloc(padded_len);
    if (!cpu_buf) {
        printf("Ошибка выделения памяти!\n");
        free(text);
        free(h_data);
        return 1;
    }
    memcpy(cpu_buf, h_data, padded_len);

    // Измеряем CPU время с высокой точностью
    QueryPerformanceCounter(&start_cpu);

    cpu_encrypt(cpu_buf, blocks, key, block_size, rounds);

    // Для проверки корректности дешифрования
    uint8_t* cpu_encrypted = (uint8_t*)malloc(padded_len);
    if (!cpu_encrypted) {
        printf("Ошибка выделения памяти!\n");
        free(text);
        free(h_data);
        free(cpu_buf);
        return 1;
    }
    memcpy(cpu_encrypted, cpu_buf, padded_len);

    cpu_decrypt(cpu_buf, blocks, key, block_size, rounds);

    QueryPerformanceCounter(&end_cpu);

    // Преобразуем в миллисекунды
    double cpu_time = ((double)(end_cpu.QuadPart - start_cpu.QuadPart) * 1000.0) / frequency.QuadPart;

    printf("CPU encryption+decryption time: %.3f ms\n", cpu_time);
    printf("Correctness CPU: %s\n", memcmp(cpu_buf, h_data, padded_len) == 0 ? "YES" : "NO");

    // GPU
    uint8_t* d_data;
    cudaError_t cudaStatus;

    cudaStatus = cudaMalloc((void**)&d_data, padded_len);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed: %s\n", cudaGetErrorString(cudaStatus));
        free(text);
        free(h_data);
        free(cpu_buf);
        free(cpu_encrypted);
        return 1;
    }

    cudaMemcpy(d_data, h_data, padded_len, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(d_sbox, sbox, 256);
    cudaMemcpyToSymbol(d_inv_sbox, inv_sbox, 256);
    cudaMemcpyToSymbol(d_key, key, 256);  // Копируем весь ключ (256 байт)

    dim3 threads(THREADS);
    dim3 grid((blocks + THREADS - 1) / THREADS);

    cudaEvent_t start_gpu, stop_gpu;
    cudaEventCreate(&start_gpu);
    cudaEventCreate(&stop_gpu);

    cudaEventRecord(start_gpu);
    encrypt_kernel << <grid, threads >> > (d_data, blocks, block_size, rounds);

    // Для проверки корректности дешифрования, скопируем зашифрованные данные
    uint8_t* gpu_encrypted = (uint8_t*)malloc(padded_len);
    if (!gpu_encrypted) {
        printf("Ошибка выделения памяти!\n");
        free(text);
        free(h_data);
        free(cpu_buf);
        free(cpu_encrypted);
        cudaFree(d_data);
        return 1;
    }
    cudaMemcpy(gpu_encrypted, d_data, padded_len, cudaMemcpyDeviceToHost);

    // Дешифрование
    decrypt_kernel << <grid, threads >> > (d_data, blocks, block_size, rounds);
    cudaEventRecord(stop_gpu);
    cudaEventSynchronize(stop_gpu);

    float gpu_time;
    cudaEventElapsedTime(&gpu_time, start_gpu, stop_gpu);

    uint8_t* gpu_buf = (uint8_t*)malloc(padded_len);
    if (!gpu_buf) {
        printf("Ошибка выделения памяти!\n");
        free(text);
        free(h_data);
        free(cpu_buf);
        free(cpu_encrypted);
        free(gpu_encrypted);
        cudaFree(d_data);
        return 1;
    }
    cudaMemcpy(gpu_buf, d_data, padded_len, cudaMemcpyDeviceToHost);

    printf("GPU encryption+decryption time: %.3f ms\n", gpu_time);
    printf("Correctness GPU: %s\n", memcmp(gpu_buf, h_data, padded_len) == 0 ? "YES" : "NO");

    // Сравнение результатов шифрования CPU и GPU
    printf("Encryption match (CPU vs GPU): %s\n",
        memcmp(cpu_encrypted, gpu_encrypted, padded_len) == 0 ? "YES" : "NO");

    if (cpu_time > 0 && gpu_time > 0) {
        printf("GPU speedup vs CPU: %.2fx\n", cpu_time / gpu_time);
    }

    printf("\nИсходный текст: %s\n", text);
    printf("После шифрования GPU (первые 32 байта HEX): ");
    for (int i = 0; i < 32 && i < padded_len; i++)
        printf("%02X ", gpu_encrypted[i]);
    printf("\n");

    // Проверка, что дешифрованный текст совпадает с оригиналом
    printf("Расшифрованный текст (GPU): ");
    for (size_t i = 0; i < text_len; i++)
        printf("%c", gpu_buf[i]);
    printf("\n");

    // Освобождение ресурсов
    cudaFree(d_data);
    free(text);
    free(h_data);
    free(cpu_buf);
    free(cpu_encrypted);
    free(gpu_buf);
    free(gpu_encrypted);
    cudaEventDestroy(start_gpu);
    cudaEventDestroy(stop_gpu);

    // Очистка ввода
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    printf("\nНажмите Enter для выхода...");
    getchar();

    return 0;
}

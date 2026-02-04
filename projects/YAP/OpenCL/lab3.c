#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int is_prime(int n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) {
            return 0;
        }
    }
    return 1;
}

char* load_kernel_source(const char* filename) {
    FILE* f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* source = (char*)malloc(size + 1);
    fread(source, 1, size, f);
    source[size] = '\0';
    fclose(f);
    return source;
}

cl_device_id getAnyOpenCLDevice(cl_platform_id platform) {
    cl_device_id device = NULL;
    cl_uint deviceCount = 0;

    cl_device_type deviceTypes[] = {
        CL_DEVICE_TYPE_GPU,
        CL_DEVICE_TYPE_CPU,
    };

    for (int i = 0; i < sizeof(deviceTypes) / sizeof(deviceTypes[0]); i++) {
        cl_int err = clGetDeviceIDs(platform, deviceTypes[i], 1, &device, &deviceCount);
        if (err == CL_SUCCESS && deviceCount > 0) {
            char deviceName[128];
            clGetDeviceInfo(device, CL_DEVICE_NAME, 128, deviceName, NULL);
            switch (deviceTypes[i]) {
            case CL_DEVICE_TYPE_GPU: printf("GPU\n"); break;
            case CL_DEVICE_TYPE_CPU: printf("CPU\n"); break;
            }
            return device;
        }
    }

    return NULL;
}

int main() {
    int N;
    printf("vvedite N (do 100000000): ");
    scanf("%d", &N);
    if (N <= 0) {
        printf("nepravilny vvod\n");
        return 1;
    }

    int* numbers = (int*)malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) {
        numbers[i] = i + 1;
    }

    clock_t start_cpu = clock();
    int count_cpu = 0;
    for (int i = 0; i < N; i++) {
        if (is_prime(numbers[i])) {
            count_cpu++;
        }
    }
    clock_t end_cpu = clock();
    double cpu_time = ((double)(end_cpu - start_cpu)) / CLOCKS_PER_SEC;

    cl_int err;
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;

    err = clGetPlatformIDs(1, &platform, NULL);
    device = getAnyOpenCLDevice(platform);

    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    queue = clCreateCommandQueue(context, device, 0, &err);

    char* kernelSource = load_kernel_source("kernel.cl");
    if (!kernelSource) {
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        free(numbers);
        return 1;
    }

    program = clCreateProgramWithSource(context, 1, (const char**)&kernelSource, NULL, &err);
    err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);

    free(kernelSource);
    kernel = clCreateKernel(program, "check_prime", &err);

    clock_t start_gpu = clock();

    cl_mem buf_numbers = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, N * sizeof(int), numbers, &err);
    cl_mem buf_results = clCreateBuffer(context, CL_MEM_WRITE_ONLY, N * sizeof(int), NULL, &err);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &buf_numbers);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &buf_results);
    clSetKernelArg(kernel, 2, sizeof(int), &N);

    size_t global_size = N;
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);
    clFinish(queue);

    int* results = (int*)malloc(N * sizeof(int));
    err = clEnqueueReadBuffer(queue, buf_results, CL_TRUE, 0, N * sizeof(int), results, 0, NULL, NULL);
   

    int count_gpu = 0;
    for (int i = 0; i < N; i++) {
        if (results[i]) {
            count_gpu++;
        }
    }

    clock_t end_gpu = clock();
    double gpu_time = ((double)(end_gpu - start_gpu)) / CLOCKS_PER_SEC;

    printf("kol-vo prostyh chisel ot 1 do %d:\n", N);
    printf("CPU: %d\n", count_cpu);
    printf("GPU: %d\n", count_gpu);
    printf("time CPU: %f sec\n", cpu_time);
    printf("time GPU: %f sec\n", gpu_time);
    printf("speedup: %f x\n", cpu_time / gpu_time);

    clReleaseMemObject(buf_numbers);
    clReleaseMemObject(buf_results);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    free(numbers);
    free(results);

    return 0;
}
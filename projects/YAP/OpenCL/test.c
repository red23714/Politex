#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CL/cl.h>

#define MAX_SOURCE_SIZE (0x100000)

int main()
{
	printf("=== OpenCL Vector Addition ===\n");

	// Размер векторов
	const int LIST_SIZE = 1024;
	float* A = (float*)malloc(sizeof(float) * LIST_SIZE);
	float* B = (float*)malloc(sizeof(float) * LIST_SIZE);
	float* C = (float*)malloc(sizeof(float) * LIST_SIZE);

	// Инициализация векторов
	for (int i = 0; i < LIST_SIZE; i++)
	{
		A[i] = (float)i;
		B[i] = (float)(LIST_SIZE - i);
	}

	// 1. Получаем платформы
	cl_platform_id platform_id = NULL;
	cl_uint num_platforms;
	clGetPlatformIDs(1, &platform_id, &num_platforms);
	printf("Platforms found: %u\n", num_platforms);

	// 2. Получаем устройства
	cl_device_id device_id = NULL;
	cl_uint num_devices;
	clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 1, &device_id,
				   &num_devices);
	printf("Devices found: %u\n", num_devices);

	// 3. Получаем имя устройства
	char device_name[256];
	clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(device_name), device_name,
					NULL);
	printf("Using device: %s\n", device_name);

	// 4. Создаем контекст
	cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, NULL);

	// 5. Создаем очередь команд
	cl_command_queue command_queue =
		clCreateCommandQueueWithProperties(context, device_id, 0, NULL);

	// 6. Загружаем исходный код ядра
	FILE* fp = fopen("vector_add.cl", "r");
	if (!fp)
	{
		printf("Failed to load kernel\n");
		return 1;
	}

	char* source_str = (char*)malloc(MAX_SOURCE_SIZE);
	size_t source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	// 7. Создаем программу
	cl_program program =
		clCreateProgramWithSource(context, 1, (const char**)&source_str,
								  (const size_t*)&source_size, NULL);

	// 8. Компилируем программу
	clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	// 9. Создаем ядро
	cl_kernel kernel = clCreateKernel(program, "vector_add", NULL);

	// 10. Создаем буферы в памяти устройства
	cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
									  LIST_SIZE * sizeof(float), NULL, NULL);
	cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
									  LIST_SIZE * sizeof(float), NULL, NULL);
	cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
									  LIST_SIZE * sizeof(float), NULL, NULL);

	// 11. Копируем данные в буферы
	clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0,
						 LIST_SIZE * sizeof(float), A, 0, NULL, NULL);
	clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0,
						 LIST_SIZE * sizeof(float), B, 0, NULL, NULL);

	// 12. Устанавливаем аргументы ядра
	clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&a_mem_obj);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&b_mem_obj);
	clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&c_mem_obj);

	// 13. Выполняем ядро
	size_t global_item_size = LIST_SIZE;
	size_t local_item_size = 64;
	clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size,
						   &local_item_size, 0, NULL, NULL);

	// 14. Копируем результат обратно
	clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0,
						LIST_SIZE * sizeof(float), C, 0, NULL, NULL);

	// 15. Проверяем результат
	printf("\nFirst 10 results:\n");
	for (int i = 0; i < 10; i++)
	{
		printf("C[%d] = %.1f + %.1f = %.1f\n", i, A[i], B[i], C[i]);
	}

	// Проверка
	int correct = 1;
	for (int i = 0; i < LIST_SIZE; i++)
	{
		if (C[i] != A[i] + B[i])
		{
			correct = 0;
			break;
		}
	}
	printf("\nResult: %s\n", correct ? "PASS" : "FAIL");

	// 16. Освобождаем ресурсы
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseMemObject(a_mem_obj);
	clReleaseMemObject(b_mem_obj);
	clReleaseMemObject(c_mem_obj);
	clReleaseCommandQueue(command_queue);
	clReleaseContext(context);

	free(A);
	free(B);
	free(C);
	free(source_str);

	return 0;
}

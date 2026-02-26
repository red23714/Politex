#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

void matmul_cpu(const float* A, const float* B, float* C, int N)
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			float sum = 0.0f;
			for (int k = 0; k < N; k++)
				sum += A[i * N + k] * B[k * N + j];
			C[i * N + j] = sum;
		}
	}
}

const char* kernelSourceCode =
	"__kernel void matmul(__global const float* A, __global const float* B, "
	"__global float* C, const int N) {\n"
	"    int row = get_global_id(0);\n"
	"    int col = get_global_id(1);\n"
	"    if (row >= N || col >= N) return;\n"
	"    float sum = 0.0f;\n"
	"    for (int k = 0; k < N; k++)\n"
	"        sum += A[row * N + k] * B[k * N + col];\n"
	"    C[row * N + col] = sum;\n"
	"}\n";

int main()
{
	int N;
	printf("enter matrix size N (NxN): ");
	if (scanf("%d", &N) != 1 || N <= 0)
	{
		printf("invalid input\n");
		exit(-1);
	}

	size_t bytes = N * N * sizeof(float);

	float* A = (float*)malloc(bytes);
	float* B = (float*)malloc(bytes);
	float* C_cpu = (float*)malloc(bytes);

	if (!A || !B || !C_cpu)
	{
		printf("memory allocation error\n");
		exit(-1);
	}

	for (int i = 0; i < N * N; i++)
	{
		A[i] = (float)(i % 100);
		B[i] = (float)((i * 2) % 100);
	}

	clock_t start_cpu = clock();
	matmul_cpu(A, B, C_cpu, N);
	double cpu_time = ((double)(clock() - start_cpu)) / CLOCKS_PER_SEC;

	cl_uint numPlatforms = 0;
	cl_platform_id* platforms;
	cl_int status = clGetPlatformIDs(0, NULL, &numPlatforms);
	if (status != CL_SUCCESS)
	{
		printf("clGetPlatformIDs failed\n");
		exit(-1);
	}

	platforms = (cl_platform_id*)malloc(numPlatforms * sizeof(cl_platform_id));
	if (platforms == NULL)
	{
		printf("malloc platform error\n");
		exit(-1);
	}
	status = clGetPlatformIDs(numPlatforms, platforms, NULL);
	if (status != CL_SUCCESS)
	{
		printf("clGetPlatformIDs failed\n");
		exit(-1);
	}

	int selected = 0;
	for (int i = 0; i < numPlatforms; i++)
	{
		char buf[100];
		status = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR,
								   sizeof(buf), buf, NULL);
		if (status == CL_SUCCESS &&
			(strstr(buf, "NVIDIA Corporation") != NULL ||
			 strstr(buf, "Advanced Micro Devices") != NULL))
		{
			selected = i;
			printf("%s\n", buf);
			break;
		}
	}

	cl_uint numDevices = 0;
	cl_device_id* devices;
	status = clGetDeviceIDs(platforms[selected], CL_DEVICE_TYPE_ALL, 0, NULL,
							&numDevices);
	if (status != CL_SUCCESS)
	{
		printf("clGetDeviceIDs error\n");
		exit(-1);
	}
	if (numDevices == 0)
	{
		printf("no devices found\n");
		exit(-1);
	}

	devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
	if (devices == NULL)
	{
		printf("malloc devices error\n");
		exit(-1);
	}
	status = clGetDeviceIDs(platforms[selected], CL_DEVICE_TYPE_ALL, numDevices,
							devices, NULL);
	if (status != CL_SUCCESS)
	{
		printf("clGetDeviceIDs failed\n");
		exit(-1);
	}

	cl_device_id device = NULL;
	for (cl_uint i = 0; i < numDevices; i++)
	{
		cl_device_type type;
		clGetDeviceInfo(devices[i], CL_DEVICE_TYPE, sizeof(type), &type, NULL);
		if (type == CL_DEVICE_TYPE_GPU)
		{
			device = devices[i];
			printf("GPU\n");
			break;
		}
	}
	if (!device)
	{
		device = devices[0];
		printf("CPU\n");
	}

	cl_context context;
	context = clCreateContext(NULL, 1, &device, NULL, NULL, &status);
	if (status != CL_SUCCESS || context == NULL)
	{
		printf("clCreateContext error\n");
		exit(-1);
	}

	cl_command_queue queue;
	queue = clCreateCommandQueue(context, device, 0, &status);
	if (status != CL_SUCCESS || queue == NULL)
	{
		printf("clCreateCommandQueue error\n");
		exit(-1);
	}

	cl_program program;
	program = clCreateProgramWithSource(
		context, 1, (const char**)&kernelSourceCode, NULL, &status);
	if (status != CL_SUCCESS)
	{
		printf("clCreateProgramWithSource error\n");
		exit(-1);
	}

	cl_int buildErr;
	buildErr = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
	if (buildErr != CL_SUCCESS)
	{
		size_t logSize;
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL,
							  &logSize);
		char* log = (char*)malloc(logSize);
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize,
							  log, NULL);
		printf("build error:\n%s\n", log);
		exit(-1);
	}

	cl_kernel kernel;
	kernel = clCreateKernel(program, "matmul", &status);
	if (status != CL_SUCCESS)
	{
		printf("clCreateKernel error\n");
		exit(-1);
	}

	clock_t start_gpu = clock();

	cl_mem dA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
							   bytes, A, &status);
	cl_mem dB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
							   bytes, B, &status);
	cl_mem dC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, bytes, 0, &status);
	if (status != CL_SUCCESS)
	{
		printf("buffer error\n");
		exit(-1);
	}

	clSetKernelArg(kernel, 0, sizeof(cl_mem), &dA);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), &dB);
	clSetKernelArg(kernel, 2, sizeof(cl_mem), &dC);
	clSetKernelArg(kernel, 3, sizeof(int), &N);

	size_t globalWorkSize[2] = {(size_t)N, (size_t)N};
	status = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalWorkSize,
									NULL, 0, NULL, NULL);
	if (status != CL_SUCCESS)
	{
		printf("clEnqueueNDRangeKernel error\n");
		exit(-1);
	}

	float* C_gpu = (float*)malloc(bytes);
	status =
		clEnqueueReadBuffer(queue, dC, CL_TRUE, 0, bytes, C_gpu, 0, NULL, NULL);
	if (status != CL_SUCCESS)
	{
		printf("read results error\n");
		exit(-1);
	}

	double gpu_time = ((double)(clock() - start_gpu)) / CLOCKS_PER_SEC;

	/* ===== correctness check ===== */
	float max_diff = 0.0f;
	for (int i = 0; i < N * N; i++)
	{
		float diff = fabsf(C_cpu[i] - C_gpu[i]);
		if (diff > max_diff)
			max_diff = diff;
	}

	if (max_diff < 1e-3f)
		printf("verification: OK (max diff = %f)\n", max_diff);
	else
		printf("verification: MISMATCH (max diff = %f)\n", max_diff);
	/* ============================= */

	printf("matrix multiplication %dx%d\n", N, N);
	printf("time CPU: %f sec\n", cpu_time);
	printf("time GPU: %f sec\n", gpu_time);
	printf("speedup: %f x\n", cpu_time / gpu_time);

	free(C_gpu);
	free(C_cpu);
	free(A);
	free(B);

	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseMemObject(dA);
	clReleaseMemObject(dB);
	clReleaseMemObject(dC);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);
	free(devices);
	free(platforms);

	return 0;
}

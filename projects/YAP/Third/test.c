#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int is_prime(int n)
{
	if (n < 2)
		return 0;
	if (n == 2)
		return 1;
	if (n % 2 == 0)
		return 0;
	for (int i = 3; i <= n / i; i += 2)
	{
		if (n % i == 0)
			return 0;
	}
	return 1;
}

const char* kernelSourceCode =
	"__kernel void check_prime(__global const int* numbers, __global int* "
	"is_prime, const int N) {\n"
	"    int idx = get_global_id(0);\n"
	"    if (idx >= N) return;\n"
	"    int n = numbers[idx];\n"
	"    if (n < 2) { is_prime[idx] = 0; return; }\n"
	"    int prime = 1;\n"
	"    if (n == 2) prime = 1;\n"
	"    else if (n % 2 == 0) prime = 0;\n"
	"    else {\n"
	"        for (int i = 3; i <= n / i; i += 2) {\n"
	"            if (n % i == 0) { prime = 0; break; }\n"
	"        }\n"
	"    }\n"
	"    is_prime[idx] = prime;\n"
	"}\n";

int main()
{
	int N;
	printf("vvedite N (do 1000000000): ");
	if (scanf("%d", &N) != 1 || N <= 0 || N > 1000000000)
	{
		printf("nepravilny vvod\n");
		exit(-1);
	}

	int* numbers = (int*)malloc(N * sizeof(int));
	if (!numbers)
	{
		printf("oshibka vydeleniya pamyati dlya massiva chisel\n");
		exit(-1);
	}

	for (int i = 0; i < N; i++)
	{
		numbers[i] = i + 1;
	}

	clock_t start_cpu = clock();
	int count_cpu = 0;
	for (int i = 0; i < N; i++)
	{
		if (is_prime(numbers[i]))
			count_cpu++;
	}
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
		printf("oshibka malloc platform\n");
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
			(strstr(buf, "AMD") != NULL ||
			 strstr(buf, "Advanced Micro Devices") != NULL))
		{
			selected = i;
			break;
		}
	}

	cl_uint numDevices = 0;
	cl_device_id* devices;
	status = clGetDeviceIDs(platforms[selected], CL_DEVICE_TYPE_ALL, 0, NULL,
							&numDevices);
	if (status != CL_SUCCESS)
	{
		printf("clGetDeviceIDs oshibka\n");
		exit(-1);
	}
	if (numDevices == 0)
	{
		printf("ustroistva ne naideny\n");
		exit(-1);
	}

	devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
	if (devices == NULL)
	{
		printf("oshibka malloc ustroistv\n");
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
		printf("clCreateContext oshibka\n");
		exit(-1);
	}

	cl_command_queue queue;
	queue = clCreateCommandQueue(context, device, 0, &status);
	if (status != CL_SUCCESS || queue == NULL)
	{
		printf("clCreateCommandQueue oshibka\n");
		exit(-1);
	}

	cl_program program;
	program = clCreateProgramWithSource(
		context, 1, (const char**)&kernelSourceCode, NULL, &status);
	if (status != CL_SUCCESS)
	{
		printf("clCreateProgramWithSource oshibka\n");
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
		printf("oshibka sborki:\n%s\n", log);
		exit(-1);
	}

	cl_kernel kernel;
	kernel = clCreateKernel(program, "check_prime", &status);
	if (status != CL_SUCCESS)
	{
		printf("clCreateKernel oshibka\n");
		exit(-1);
	}

	clock_t start_gpu = clock();

	cl_mem hDeviceMemNumbers, hDeviceMemResults;
	hDeviceMemNumbers =
		clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
					   N * sizeof(int), numbers, &status);
	hDeviceMemResults =
		clCreateBuffer(context, CL_MEM_WRITE_ONLY, N * sizeof(int), 0, &status);
	if (status != CL_SUCCESS || hDeviceMemResults == NULL)
	{
		printf("oshibka buferov\n");
		exit(-1);
	}

	clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&hDeviceMemNumbers);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&hDeviceMemResults);
	clSetKernelArg(kernel, 2, sizeof(int), &N);

	size_t globalWorkSize, localWorkSize;
	globalWorkSize = N;
	localWorkSize = 0;
	status = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalWorkSize,
									NULL, 0, NULL, NULL);
	if (status != CL_SUCCESS)
	{
		printf("clEnqueueNDRangeKernel oshibka\n");
		exit(-1);
	}

	clEnqueueReadBuffer(queue, hDeviceMemResults, CL_TRUE, 0, N * sizeof(int),
						(int*)malloc(N * sizeof(int)), 0, 0, 0);

	int* results = (int*)malloc(N * sizeof(int));
	if (!results)
	{
		printf("oshibka pamyati rezultatov\n");
		exit(-1);
	}
	status = clEnqueueReadBuffer(queue, hDeviceMemResults, CL_TRUE, 0,
								 N * sizeof(int), results, 0, NULL, NULL);
	if (status != CL_SUCCESS)
	{
		printf("oshibka chteniya rezultatov\n");
		exit(-1);
	}

	int count_gpu = 0;
	for (int i = 0; i < N; i++)
	{
		if (results[i])
			count_gpu++;
	}
	double gpu_time = ((double)(clock() - start_gpu)) / CLOCKS_PER_SEC;

	printf("kol-vo prostyh chisel ot 1 do %d:\n", N);
	printf("CPU: %d\n", count_cpu);
	printf("GPU: %d\n", count_gpu);
	printf("time CPU: %f sec\n", cpu_time);
	printf("time GPU: %f sec\n", gpu_time);
	printf("speedup: %f x\n", cpu_time / gpu_time);

	free(results);
	free(numbers);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseMemObject(hDeviceMemNumbers);
	clReleaseMemObject(hDeviceMemResults);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);
	free(devices);
	free(platforms);

	return 0;
}

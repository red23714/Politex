#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define K 15
#define R (K / 2)

const char* kernelSourceCode =
	"__kernel void conv2d(__global const float* input,\n"
	"                     __global const float* filter,\n"
	"                     __global float* output,\n"
	"                     const int N)\n"
	"{\n"
	"    int idx = get_global_id(0);\n"
	"    int total = N*N;\n"
	"    if (idx >= total) return;\n"
	"\n"
	"    int x = idx % N;\n"
	"    int y = idx / N;\n"
	"\n"
	"    float sum = 0.0f;\n"
	"    for(int ky=-7; ky<=7; ky++){\n"
	"        for(int kx=-7; kx<=7; kx++){\n"
	"            int nx = x + kx;\n"
	"            int ny = y + ky;\n"
	"            if(nx>=0 && nx<N && ny>=0 && ny<N){\n"
	"                float v = input[ny*N + nx];\n"
	"                float w = filter[(ky+7)*15 + (kx+7)];\n"
	"                sum += v*w;\n"
	"            }\n"
	"        }\n"
	"    }\n"
	"    output[idx] = sum;\n"
	"}\n";

int main()
{
	int N;
	printf("Enter N (image NxN): ");
	if (scanf("%d", &N) != 1 || N <= 0)
		return 0;

	int total = N * N;

	float* image = (float*)malloc(total * sizeof(float));
	float* kernel = (float*)malloc(K * K * sizeof(float));
	float* cpu_out = (float*)malloc(total * sizeof(float));

	for (int i = 0; i < total; i++)
		image[i] = (float)rand() / RAND_MAX;

	for (int i = 0; i < K * K; i++)
		kernel[i] = (float)rand() / RAND_MAX;

	/* ================= CPU ================= */
	clock_t t0 = clock();

	for (int y = 0; y < N; y++)
		for (int x = 0; x < N; x++)
		{
			float sum = 0.0f;
			for (int ky = -R; ky <= R; ky++)
				for (int kx = -R; kx <= R; kx++)
				{
					int nx = x + kx, ny = y + ky;
					if (nx >= 0 && nx < N && ny >= 0 && ny < N)
					{
						float v = image[ny * N + nx];
						float w = kernel[(ky + R) * K + (kx + R)];
						sum += v * w;
					}
				}
			cpu_out[y * N + x] = sum;
		}

	double cpu_time = (double)(clock() - t0) / CLOCKS_PER_SEC;

	cl_uint numPlatforms = 0;
	clGetPlatformIDs(0, NULL, &numPlatforms);

	cl_platform_id* platforms =
		(cl_platform_id*)malloc(numPlatforms * sizeof(cl_platform_id));
	clGetPlatformIDs(numPlatforms, platforms, NULL);

	int selected = 0;
	for (int i = 0; i < numPlatforms; i++)
	{
		char buf[100];
		clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(buf), buf,
						  NULL);
		if (strstr(buf, "AMD") || strstr(buf, "Advanced Micro Devices"))
		{
			selected = i;
			break;
		}
	}

	cl_uint numDevices = 0;
	clGetDeviceIDs(platforms[selected], CL_DEVICE_TYPE_ALL, 0, NULL,
				   &numDevices);

	cl_device_id* devices =
		(cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
	clGetDeviceIDs(platforms[selected], CL_DEVICE_TYPE_ALL, numDevices, devices,
				   NULL);

	cl_device_id device = NULL;
	for (cl_uint i = 0; i < numDevices; i++)
	{
		cl_device_type type;
		clGetDeviceInfo(devices[i], CL_DEVICE_TYPE, sizeof(type), &type, NULL);
		if (type == CL_DEVICE_TYPE_GPU)
		{
			device = devices[i];
			printf("GPU selected\n");
			break;
		}
	}
	if (!device)
	{
		device = devices[0];
		printf("CPU selected\n");
	}

	cl_int status;
	cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &status);

	cl_command_queue queue = clCreateCommandQueue(context, device, 0, &status);

	cl_program program =
		clCreateProgramWithSource(context, 1, &kernelSourceCode, NULL, &status);

	status = clBuildProgram(program, 1, &device, NULL, NULL, NULL);

	if (status != CL_SUCCESS)
	{
		size_t logSize;
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL,
							  &logSize);
		char* log = (char*)malloc(logSize);
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize,
							  log, NULL);
		printf("Build error:\n%s\n", log);
		return 0;
	}

	cl_kernel k = clCreateKernel(program, "conv2d", &status);

	clock_t tg = clock();

	cl_mem d_image =
		clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
					   total * sizeof(float), image, &status);

	cl_mem d_kernel =
		clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
					   K * K * sizeof(float), kernel, &status);

	cl_mem d_out = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
								  total * sizeof(float), NULL, &status);

	clSetKernelArg(k, 0, sizeof(cl_mem), &d_image);
	clSetKernelArg(k, 1, sizeof(cl_mem), &d_kernel);
	clSetKernelArg(k, 2, sizeof(cl_mem), &d_out);
	clSetKernelArg(k, 3, sizeof(int), &N);

	size_t globalWorkSize = total;
	clEnqueueNDRangeKernel(queue, k, 1, NULL, &globalWorkSize, NULL, 0, NULL,
						   NULL);
	clFinish(queue);

	float* gpu_out = (float*)malloc(total * sizeof(float));
	clEnqueueReadBuffer(queue, d_out, CL_TRUE, 0, total * sizeof(float),
						gpu_out, 0, NULL, NULL);

	double gpu_time = (double)(clock() - tg) / CLOCKS_PER_SEC;

	/* ================= checksum ================= */

	double s1 = 0, s2 = 0;
	for (int i = 0; i < total; i++)
	{
		s1 += cpu_out[i];
		s2 += gpu_out[i];
	}

	printf("checksum CPU: %.6f\n", s1);
	printf("checksum GPU: %.6f\n", s2);
	printf("time CPU: %.3f sec\n", cpu_time);
	printf("time GPU: %.3f sec\n", gpu_time);
	printf("speedup: %.2fx\n", cpu_time / gpu_time);

	return 0;
}

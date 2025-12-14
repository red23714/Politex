#define CL_TARGET_OPENCL_VERSION 120
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define WIDTH 4096
#define HEIGHT 4096
#define ITER 2000

double cpu_time_sec() { return (double)clock() / CLOCKS_PER_SEC; }

const char* kernelSource =
	"__kernel void mandelbrot(__global int* output,            \n"
	"                         int width, int height, int iter)\n"
	"{                                                         \n"
	"    int x = get_global_id(0);                             \n"
	"    int y = get_global_id(1);                             \n"
	"    if (x >= width || y >= height) return;                \n"
	"                                                          \n"
	"    double cx = (x - width  * 0.5) * 4.0 / width;         \n"
	"    double cy = (y - height * 0.5) * 4.0 / height;        \n"
	"    double zx = 0.0, zy = 0.0;                             \n"
	"    int i;                                                 \n"
	"                                                          \n"
	"    for (i = 0; i < iter; i++) {                           \n"
	"        double zx2 = zx*zx - zy*zy + cx;                   \n"
	"        zy = 2.0*zx*zy + cy;                               \n"
	"        zx = zx2;                                          \n"
	"        if (zx*zx + zy*zy > 4.0) break;                    \n"
	"    }                                                      \n"
	"    output[y * width + x] = i;                             \n"
	"}                                                         \n";

int main()
{
	size_t pixels = (size_t)WIDTH * HEIGHT;
	int* cpu = malloc(sizeof(int) * pixels);
	int* gpu = malloc(sizeof(int) * pixels);

	// ================= CPU =================
	double t0 = cpu_time_sec();

	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			double cx = (x - WIDTH * 0.5) * 4.0 / WIDTH;
			double cy = (y - HEIGHT * 0.5) * 4.0 / HEIGHT;
			double zx = 0.0, zy = 0.0;
			int i;
			for (i = 0; i < ITER; i++)
			{
				double zx2 = zx * zx - zy * zy + cx;
				zy = 2.0 * zx * zy + cy;
				zx = zx2;
				if (zx * zx + zy * zy > 4.0)
					break;
			}
			cpu[y * WIDTH + x] = i;
		}
	}

	double t1 = cpu_time_sec();
	double cpu_time = t1 - t0;

	printf("CPU time: %.3f sec\n", cpu_time);

	// ================= OpenCL =================
	cl_platform_id platform;
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;

	clGetPlatformIDs(1, &platform, NULL);
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);

	context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
	queue =
		clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, NULL);

	cl_mem out = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
								sizeof(int) * pixels, NULL, NULL);

	cl_program program =
		clCreateProgramWithSource(context, 1, &kernelSource, NULL, NULL);

	clBuildProgram(program, 1, &device, "-cl-fast-relaxed-math", NULL, NULL);

	cl_kernel kernel = clCreateKernel(program, "mandelbrot", NULL);

	int width = WIDTH;
	int height = HEIGHT;
	int iter = ITER;

	clSetKernelArg(kernel, 0, sizeof(cl_mem), &out);
	clSetKernelArg(kernel, 1, sizeof(int), &width);
	clSetKernelArg(kernel, 2, sizeof(int), &height);
	clSetKernelArg(kernel, 3, sizeof(int), &iter);

	size_t global[2] = {(size_t)width, (size_t)height};

	cl_event event;
	clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, NULL, 0, NULL,
						   &event);
	clFinish(queue);

	cl_ulong start, end;
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(start),
							&start, NULL);
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(end), &end,
							NULL);

	double gpu_time = (end - start) * 1e-9;

	clEnqueueReadBuffer(queue, out, CL_TRUE, 0, sizeof(int) * pixels, gpu, 0,
						NULL, NULL);

	printf("GPU kernel time: %.3f sec\n", gpu_time);
	printf("Speedup: %.1fx\n", cpu_time / gpu_time);

	printf("Verification: %s\n", cpu[123] == gpu[123] ? "OK" : "FAILED");

	return 0;
}

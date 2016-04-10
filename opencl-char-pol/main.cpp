#define  _CRT_SECURE_NO_WARNINGS
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <cstring>
#include <omp.h>
#include <fstream>
using namespace std::chrono;
#define CHECK_OPENCL_ERROR(actual, msg)                                        \
    if(actual != CL_SUCCESS)                                                   \
		{                                                                      \
        std::cout << "Location : " << __FILE__ << ":" << __LINE__<< std::endl; \
        system("PAUSE"); exit(-1);                                            \
	    }

#define SUCCESS 0
#define FAILURE 1
#define EXPECTED_FAILURE 2

#define  GlobalThreadSize 256
#define  GroupSize 1

float get(float *matrix, int n, int x, int y) {
	if (x >= n || y >= n) {
		printf("\nAhtung! get %d %d\n", x, y);
		return 0.0;
	}
	//printf("get %d %d\n", x, y);
	return (float)matrix[y * n + x];
}

void set(float *matrix, int n, int x, int y, float value) {
	if (x >= n || y >= n) {
		printf("\nAhtung! set %d %d\n", x, y);
		return;
	}
	matrix[y * n + x] = (float)value;
}

void print_matrix(float *matrix, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			printf("%f ", get(matrix, n, j, i));
		}
		printf("\n");
	}

	printf("\n");
}

// sum of the diagonal elements
float trace(float *m, int n) {
	float sum = 0.0;

	for (int i = 0; i < n; i++) {
		sum += get(m, n, i, i);
	}
	return sum;
}

void mult_matrix(float *a, float *b, float *mult, int n) {
	//float* mult = (float *) malloc(n * n * sizeof(float));
	float sum = 0;

	for (int c = 0; c < n; c++) {
		for (int d = 0; d < n; d++) {
			for (int k = 0; k < n; k++) {
				sum = sum + get(b, n, c, k) * get(a, n, k, d);
			}
			set(mult, n, c, d, sum);
			sum = 0;
		}
	}
}

// fills diagonal with mult_on
void fill_identity(float* m, int n, float mult_on)
{
	for (int i = 0; i < n; i++)
	{
		set(m, n, i, i, mult_on);
	}
}

// Finding the characteristic polynomial of a matrix of order n
float* char_pol(float* mat, int n)
{
	float* pol = (float*)malloc(sizeof(float) * (n + 1));
	for (int i = 0; i < n + 1; i++) pol[n] = 1.0f;
	float* c = (float*)malloc(sizeof(float) * n * n);
	float* c_plus_ident = (float*)malloc(sizeof(float) * n * n);
	float* b = (float*)malloc(sizeof(float) * n * n);
	for (int i = 0; i < n*n; i++) b[i] = mat[i];
	float * ident = (float*)malloc(sizeof(float) * n * n);


	for (int k = 0; k < n; k++)
	{
		for (int i = 0; i < n*n; i++) c[i] = b[i];
		float p = -trace(c, n) / (k + 1);
		pol[n - k - 1] = p;
		fill_identity(ident, n, p);
		for (int i = 0; i < n*n; i++) c_plus_ident[i] = c[i] + ident[i];
		mult_matrix(mat, c_plus_ident, b, n);
	}

	free(c);
	free(c_plus_ident);
	free(ident);
	free(b);
	return pol;
}


void log_error(cl_program program, cl_device_id device_id)
{
	// Determine the size of the log
	size_t log_size;
	int status = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
	if (status != CL_SUCCESS) std::cout << "clGetProgramBuildInfo ERROR : code " << status << std::endl;
	// Allocate memory for the log
	char *log = (char *)malloc(log_size + 1);

	// Get the log
	status = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
	if (status != CL_SUCCESS) std::cout << "clGetProgramBuildInfo ERROR : code " << status << std::endl;

	// Print the log
	printf("%s\n", log);
}

int convertToString(const char *filename, std::string& s)
{
	size_t size;
	char*  str;
	std::fstream f(filename, (std::fstream::in | std::fstream::binary));

	if (f.is_open())
	{
		size_t fileSize;
		f.seekg(0, std::fstream::end);
		size = fileSize = (size_t)f.tellg();
		f.seekg(0, std::fstream::beg);
		str = new char[size + 1];
		if (!str)
		{
			f.close();
			return SUCCESS;
		}

		f.read(str, fileSize);
		f.close();
		str[size] = '\0';
		s = str;
		delete[] str;
		return SUCCESS;
	}
	std::cout << "Error: failed to open file\n:" << filename << std::endl;
	system("PAUSE"); return FAILURE;
}

size_t mult_global_threads[2];
size_t mult_local_threads[2];
cl_command_queue commandQueue;
cl_kernel kernel_mult;
cl_mem mult_a_buffer;
cl_mem mult_b_buffer;
cl_mem mult_c_buffer;

void gpu_mult(float* a, float *b, float *c, int n)
{
	mult_global_threads[0] = n;
	mult_global_threads[1] = n;
	mult_local_threads[0] = 1;
	mult_local_threads[1] = 1;
	int status;
	int inputSizeBytes = n*n*sizeof(cl_float);
	status = clEnqueueWriteBuffer(commandQueue, mult_a_buffer, CL_TRUE, 0, inputSizeBytes, a, 0, 0, 0);
	CHECK_OPENCL_ERROR(status, "Error: Enqueue write buffer");
	status = clEnqueueWriteBuffer(commandQueue, mult_b_buffer, CL_TRUE, 0, inputSizeBytes, b, 0, 0, 0);
	CHECK_OPENCL_ERROR(status, "Error: Enqueue write buffer");

	status = clEnqueueNDRangeKernel(commandQueue, kernel_mult, 2, NULL, mult_global_threads, mult_local_threads, 0, NULL, NULL);
	CHECK_OPENCL_ERROR(status, "Error: Enqueue kernel onto command queue failed");
	status = clFinish(commandQueue);
	CHECK_OPENCL_ERROR(status, "Error: clFinish fail.");
	status = clEnqueueReadBuffer(commandQueue, mult_c_buffer, CL_FALSE, 0, inputSizeBytes, c, 0, 0, 0);
	CHECK_OPENCL_ERROR(status, "Error: Enqueue read buffer failed");
	status = clFinish(commandQueue);
	CHECK_OPENCL_ERROR(status, "Error: clFinish");
}


float* gpu_char_pol(float* mat, int n)
{
	float* pol = (float*)malloc(sizeof(float) * (n + 1));
	
	for (int i = 0; i < n + 1; i++) pol[n] = 1.0f;
	float* c = (float*)malloc(sizeof(float) * n * n);
	float* c_plus_ident = (float*)malloc(sizeof(float) * n * n);
	float* b = (float*)malloc(sizeof(float) * n * n);
	for (int i = 0; i < n*n; i++) b[i] = mat[i];
	float * ident = (float*)malloc(sizeof(float) * n * n);

	for (int k = 0; k < n; k++)
	{
		for (int i = 0; i < n*n; i++) c[i] = b[i];
		float sum = trace(c, n);
		printf("GPU : %f\n", sum);
		float p = - sum/ (k + 1);
		fill_identity(ident, n, p);
		for (int i = 0; i < n*n; i++)  c_plus_ident[i] = c[i] + ident[i];
		pol[n - k - 1] = p;
		gpu_mult(mat, c_plus_ident, b, n);
	}

	free(c);
	free(c_plus_ident);
	free(ident);
	free(b);
	return pol;
}

float compute_omp(float* c, float* b, float* ident, float* c_plus_ident, int k, int n)
{
	//for (int i = 0; i < n*n; i++) c[i] = b[i];

#pragma omp parallel for
	for (int i = 0; i < n*n; i++) c[i] = b[i];
	
	float p = - trace(c, n) / (k + 1);
	fill_identity(ident, n, p);

#pragma omp parallel for
	for (int i = 0; i < n*n; i++)  c_plus_ident[i] = c[i] + ident[i];
	
	return p;
}


float* opm_gpu_char_pol(float* mat, int n)
{
	float* pol = (float*)malloc(sizeof(float) * (n + 1));

	for (int i = 0; i < n + 1; i++) pol[n] = 1.0f;
	float* c = (float*)malloc(sizeof(float) * n * n);
	float* c_plus_ident = (float*)malloc(sizeof(float) * n * n);
	float* b = (float*)malloc(sizeof(float) * n * n);
	for (int i = 0; i < n*n; i++) b[i] = mat[i];
	float * ident = (float*)malloc(sizeof(float) * n * n);

	for (int k = 0; k < n; k++)
	{
		/*for (int i = 0; i < n*n; i++) c[i] = b[i];
		float p = -trace(c, n) / (k + 1);
		fill_identity(ident, n, p);
		for (int i = 0; i < n*n; i++)  c_plus_ident[i] = c[i] + ident[i];*/
		float p = compute_omp(c, b, ident, c_plus_ident, k, n);
		pol[n - k - 1] = p;
		gpu_mult(mat, c_plus_ident, b, n);
	}

	free(c);
	free(c_plus_ident);
	free(ident);
	free(b);
	return pol;
}

int create_matrix_from_file(float* &mat_ptr, const char* filename)
{
	FILE *fp;
	fp = fopen(filename, "r");
	if (fp == NULL) { printf("File open error\n"); exit(-1); }
	int n = 0;
	fscanf(fp, "%d", &n);
	fscanf(fp, "%d", &n);
	float *mat = (float*)malloc(sizeof(float) * n * n);
	for (int i = 0; i < n*n; i++)
	{
		fscanf(fp, "%f", &mat[i]);
		//printf("%f ", mat[i]);
	}
	fclose(fp);
	mat_ptr = mat;
	return n;
}

int main() {

	//time_t start = time(0);
	printf("Reading matrix from file...\n");
	int size = 0;
	float* mat;
	size = create_matrix_from_file(mat, "matrix_10_100x100f.txt");

	int blocks = size * size;

	cl_int status = 0;//store the return status
	cl_uint numPlatforms;//store the number of platforms query by clGetPlatformIDs()
	cl_platform_id platform = NULL;//store the chosen platform

	//get platform 
	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	CHECK_OPENCL_ERROR(status, "Error: Getting platforms");
	

	if (numPlatforms > 0)
	{
		cl_platform_id* platforms = (cl_platform_id*)malloc(numPlatforms* sizeof(cl_platform_id));
		status = clGetPlatformIDs(numPlatforms, platforms, NULL);
		platform = platforms[0];
		free(platforms);
	}

	if (NULL == platform)
	{
		std::cout << "Error: No available platform found!" << std::endl;
		system("PAUSE"); return FAILURE;
	}



	cl_uint numDevice = 0;
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &numDevice);
	cl_device_id *devices = (cl_device_id*)malloc(numDevice*sizeof(cl_device_id));
	if (devices == 0)
	{
		std::cout << "No device available\n";
		system("PAUSE"); return FAILURE;
	}
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, numDevice, devices, NULL);


	cl_context context = clCreateContext(NULL, numDevice, devices, NULL, NULL, NULL);
	CHECK_OPENCL_ERROR(status, "Error: Creating context failed");

	 commandQueue = clCreateCommandQueue(context, devices[0], 0, &status);
	CHECK_OPENCL_ERROR(status, "Error: Creating command queue failed");


	//set input data
	cl_uint inputSizeBytes = blocks *  sizeof(cl_float);
	

	//////////////////
	// mult buffers //
	//////////////////
	cl_float* c_mem = (cl_float*)malloc(inputSizeBytes);
	cl_float* a_mem = (cl_float*)malloc(inputSizeBytes);
	cl_float* b_mem = (cl_float*)malloc(inputSizeBytes);

	 mult_c_buffer = clCreateBuffer(
		context,
		CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
		inputSizeBytes,
		(void*)c_mem,
		&status);
	CHECK_OPENCL_ERROR(status, "Error: Creating output buffer failed");

	 mult_a_buffer = clCreateBuffer(
		context,
		CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
		inputSizeBytes,
		(void *)a_mem,
		&status);

	CHECK_OPENCL_ERROR(status, "Error: Creating input buffer failed");

	 mult_b_buffer = clCreateBuffer(
		context,
		CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
		inputSizeBytes,
		(void *)b_mem,
		&status);

	CHECK_OPENCL_ERROR(status, "Error: Creating input buffer failed");

	/////////////////////////////////////////////////////


	const char* filename2 = "./kernel.cl";
	std::string sourceStr2;
	status = convertToString(filename2, sourceStr2);
	const char *source2 = sourceStr2.c_str();
	size_t sourceSize2[] = { strlen(source2) };

	cl_program program = clCreateProgramWithSource(context, 1, &source2, sourceSize2, &status);
	CHECK_OPENCL_ERROR(status, "Error: Creating program object failed");

	status = clBuildProgram(program, 1, devices, NULL, NULL, NULL);

	if (status != CL_SUCCESS)
	{
		std::cout << "Error: Building program error code " << status << std::endl;
		log_error(program, devices[1]);
		system("PAUSE"); return FAILURE;
	}

	kernel_mult = clCreateKernel(program,"matrixMul", &status);
	CHECK_OPENCL_ERROR(status, "Error: Creating matrixMul failed");

	int  n = size;
	status = clSetKernelArg(kernel_mult, 0, sizeof(cl_mem), (void *)&mult_c_buffer);
	status = clSetKernelArg(kernel_mult, 1, sizeof(cl_mem), (void *)&mult_a_buffer);
	status = clSetKernelArg(kernel_mult, 2, sizeof(cl_mem), (void *)&mult_b_buffer);
	status = clSetKernelArg(kernel_mult, 3, sizeof(int), (void*)&n);
	
	printf("Computing on CPU...\n");
	auto start = system_clock::now();
	float* pol = char_pol(mat, size);
	auto end = system_clock::now();
	auto cpu_total = duration_cast<nanoseconds>(end - start).count()/1000000.0;
	//cpu_total = start - time(0);
	
	//start = time(0);
	printf("Computing on GPU...\n");
	start = system_clock::now();
	float* gpu_pol = gpu_char_pol(mat, size);
	end = system_clock::now();
	auto gpu_total = duration_cast<nanoseconds>(end - start).count()/1000000.0;
	//gpu_total = start - time(0);

	printf("Computing on GPU + OPENMP...\n");
	start = system_clock::now();
	float* omp_gpu_pol = opm_gpu_char_pol(mat, size);
	end = system_clock::now();
	auto omp_gpu_total = duration_cast<nanoseconds>(end - start).count() / 1000000.0;

	float gpu_sum = 0, cpu_sum = 0, omp_gpu_sum = 0;
	for (int i = 0; i < size + 1; i++)  { 
		cpu_sum += pol[i]; 
		gpu_sum += gpu_pol[i];
		omp_gpu_sum += omp_gpu_pol[i];
		printf("%f\t %f\t %f\n", pol[i], gpu_pol[i], omp_gpu_pol[i]);
	}
	printf("Checksum:\nCPU     :\t%f\nGPU     :\t%f\nOMP+GPU :\t%f\n", cpu_sum, gpu_sum, omp_gpu_sum);
	printf("Time:\nCPU     :\t%f ms\nGPU     :\t%f ms\nOMP+GPU :\t%f\n", cpu_total, gpu_total, omp_gpu_total);

	// Clean the resources.
	status = clReleaseKernel(kernel_mult);
	status = clReleaseProgram(program);//Release program.
	status = clReleaseCommandQueue(commandQueue);//Release command queue.
	status = clReleaseContext(context);//Release context.

	if (devices != NULL)
	{
		free(devices);
		devices = NULL;
	}

	std::cout << "Passed!\n";
	free(mat);
	system("PAUSE");
	return 0;
}

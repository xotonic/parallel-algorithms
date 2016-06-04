#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <omp.h>
using namespace std;

const int log_level = 0;
#define IF_LOG if (log_level > 0)

float get(float *matrix, int n, int x, int y) {
	if (x >= n || y >= n) {
		printf("\nAhtung! get %d %d\n", x, y);
		return 0.0;
	}
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

vector<vector<int>> hvariants;
int find_variant(int start, int size, int current, int subsize, vector<int> indexes) 
{
	int found = -1;
	if (start < size && current < subsize)
	{
		found = start + 1;
		current++;
		indexes.push_back(found);
		if (current < subsize)
		while (start < size)
		{
			start++;
			find_variant(start, size, current, subsize, indexes);
		}
		else
		{
			hvariants.push_back(indexes);
		}
	}
	return found;
}

float submatrix_sum(float* mat, int x, int y, int subsize, int size)
{
	float sum = -1;
	auto h = hvariants[x];
	auto v = hvariants[y];
	for (int i = 0; i < subsize; i++)
		for (int j = 0; j < subsize; j++)
			sum += get(mat, size, v[j],h[i]);
	return sum;
}

void print_submatrix(float* mat, int x, int y, int subsize, int size)
{
	auto h = hvariants[x];
	auto v = hvariants[y];
	for (int i = 0; i < subsize; i++)
	{
		for (int j = 0; j < subsize; j++)
			printf("%f ", get(mat, size, v[j], h[i]));
		printf("\n");
	}
}
int main()
{
	int size = 0;
	int subsize = 0;
	float* mat;
	printf("Matrix file\n>>");
	char filename[255];
	scanf("%s", &filename);
	printf("Submatrix size\n>>");
	scanf("%d", &subsize);
	printf("Reading matrix from file...\n");
	size = create_matrix_from_file(mat, filename /*"matrix_10_200x200f.txt"*/);

	auto start = omp_get_wtime();

	printf("Finding submatrixes...\n");

	IF_LOG printf("Recursive test:\n");
	
	for (int i = 0; i < size - 1; i++)
	{
		find_variant(i - 1, size - 1, 0, subsize, vector<int>());
	}

	IF_LOG {
	printf("Result:\n");
	for (auto i : hvariants)
	{
		for (auto n : i)
			printf("%d ", n);
		printf("\n");
	}
	}
	printf("Finding max sum...\n");

	float max = 0.0f;
	int max_i = -1;
	int max_j = -1;
	for (int i = 0; i < hvariants.size(); i++)
		for (int j = i; j < hvariants.size(); j++)
		{
			float sum = submatrix_sum(mat, i, j, subsize, size);
			IF_LOG printf("i = %d j = %d sum = %f\n", i, j, sum);
			IF_LOG print_submatrix(mat, i, j, subsize, size);
			if (sum > max)
			{
				max = sum;
				max_i = i; max_j = j;
			}
		}
	auto end = omp_get_wtime();
	printf("Time : %f\n", end - start);
	printf("Max submatrix: sum = %f\n", max);
	//printf("Max submatrix:\nsum = %f\ni = %d j = %d\n", max, max_i, max_j);
	IF_LOG print_submatrix(mat, max_i, max_j, subsize, size);

	return 0;
}
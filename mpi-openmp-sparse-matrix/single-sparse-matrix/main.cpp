#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <omp.h>
using namespace std;

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
			sum += get(mat, size, h[i], v[j]);
	return sum;
}

void print_submatrix(float* mat, int x, int y, int subsize, int size)
{
	auto h = hvariants[x];
	auto v = hvariants[y];
	for (int i = 0; i < subsize; i++)
	{
		for (int j = 0; j < subsize; j++)
			printf("%f ", get(mat, size, h[i], v[j]));
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

	//float* hindexes = (float*)malloc(sizeof(float) * subsize);
	//float* vindexes = (float*)malloc(sizeof(float) * subsize);



	/*for (int i = 0; i < size; i++)
	{
		for (int j = i + 1; j < size; j++)
		{
			printf("%d - %d\n", i, j);
		}
	}*/

	printf("Recursive test:\n");
	
	for (int i = 0; i < size - 1; i++)
	{
		find_variant(i - 1, size - 1, 0, subsize, vector<int>());
	}

	printf("Result:\n");
	for (auto i : hvariants)
	{
		for (auto n : i)
			printf("%d ", n);
		printf("\n");
	}
	
	int max = -1;
	int max_i = -1;
	int max_j = -1;
	for (int i = 0; i < size; i++)
		for (int j = i; j < size; j++)
		{
			float sum = submatrix_sum(mat, i, j, subsize, size);
			printf("i = %d j = %d sum = %f\n", i, j, sum);
			print_submatrix(mat, i, j, subsize, size);
			if (sum > max)
			{
				max = sum;
				max_i = i; max_j = j;
			}
		}

	printf("Max submatrix:\nsum = %d\ni = %d j = %d\n", max, max_i, max_j);
	print_submatrix(mat, max_i, max_j, subsize, size);

	return 0;
}
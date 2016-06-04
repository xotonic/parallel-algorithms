#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


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

	print_matrix(mat, size);
	
	//printf("Total submatrixes : %d\n", submatrix_total);


	return 0;
}
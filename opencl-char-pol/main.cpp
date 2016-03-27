#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

float get(float *matrix, int n, int x, int y) {
    if (x >= n || y >= n) {
        printf("\nAhtung! get %d %d\n", x, y);
        return 0.0;
    }
    return matrix[y * n + x];
}

void set(float *matrix, int n, int x, int y, float value) {
    if (x >= n || y >= n) {
        printf("\nAhtung! set %d %d\n", x, y);
        return;
    }
    matrix[y * n + x] = value;
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
void fill_identity(float* m,int n, float mult_on)
{
    for (int i = 0; i < n; i++)
    {
        set(m,n,i,i, mult_on);
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


    for (int k =0; k < n; k++)
    {
        for (int i = 0; i < n*n; i++) c[i] = b[i];
        float p = -trace(c,n) / ( k + 1);
        pol[n - k - 1] = p;
        fill_identity(ident,n, p);
        for (int i =0; i < n*n; i++) c_plus_ident[i] = c[i] + ident[i];
        mult_matrix(mat,c_plus_ident, b, n);
    }
    return pol;
}

int main() {


    // ****************
    // CL BLOCK
    // ****************

    cl_uint platformIdCount = 0;
    clGetPlatformIDs(0, NULL, &platformIdCount);
    printf("CL\tPlatform Count:\t%d", platformIdCount);

    // ****************
    // SINGLE
    // ****************

    // matrix size
    int size = 0;
    printf("Enter matrix size : ");
    scanf("%d", &size);
    float *mat = (float *) malloc(size * size * sizeof(float));

    set(mat, size, 0,0, 8);
    set(mat, size, 1,0, -1);
    set(mat, size, 2,0, 3);
    set(mat, size, 3,0, -1);

    set(mat, size, 0,1, -1);
    set(mat, size, 1,1, 6);
    set(mat, size, 2,1, 2);
    set(mat, size, 3,1, 0);

    set(mat, size, 0,2, 3);
    set(mat, size, 1,2, 2);
    set(mat, size, 2,2, 9);
    set(mat, size, 3,2, 1);

    set(mat, size, 0,3, -1);
    set(mat, size, 1,3, 0);
    set(mat, size, 2,3, 1);
    set(mat, size, 3,3, 7);

    print_matrix(mat, size);
    float* pol = char_pol(mat, size);

    for (int i = 0; i < size + 1; i++) printf("\%f ", pol[i]);
    free(mat);
    return 0;
}
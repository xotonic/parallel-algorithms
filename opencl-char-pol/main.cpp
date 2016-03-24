#include <stdio.h>
#include <stdlib.h>

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

float *mult_matrix(float *a, float *b, int n) {
    float *mult = (float *) malloc(n * n * sizeof(float));
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
    return mult;
}

int main() {

    // matrix size
    int size = 0;
    printf("Enter matrix size : ");
    scanf("%d", &size);
    float *mat = (float *) malloc(size * size * sizeof(float));
    float *mat2 = (float *) malloc(size * size * sizeof(float));

    set(mat, size, 0, 1, 2.0);
    set(mat, size, 1, 0, 3.0);
    set(mat2, size, 0, 1, 4.0);
    set(mat2, size, 1, 0, 5.0);


    float *mult = mult_matrix(mat, mat2, size);

    print_matrix(mat, size);
    print_matrix(mat2, size);
    print_matrix(mult, size);
    free(mat);
    return 0;
}
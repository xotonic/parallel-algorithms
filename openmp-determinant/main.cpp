#include <iostream>
#include <fstream>
#include <math.h>
#include <omp.h>
#include <vector>
#include <chrono>
using namespace std;
using namespace std::chrono;

using Matrix = vector<vector<float>>;


Matrix submatrix(Matrix mat, int i) {
    Matrix sub;
    int sub_n = mat.size();
    for (int y = 1; y < sub_n; y++) {
        vector<float> v;

        for (int x = 0; x < sub_n; x++) {
            if (x != i) v.push_back(mat[y][x]);
        }
        sub.push_back(v);
    }
    return sub;
}

inline float det2x2(Matrix mat) {
    float det;
    if (mat.size() != 2) {
        cerr << "det2x2 : matrix not 2x2\n";
        exit(0);
    }
    det = mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1];
    return det;
}

float det2_seq(Matrix mat) {
    float det = 0;
    int n = mat.size();
    //clog << "n = " << n << endl;

    if (n == 2) det = det2x2(mat);
    else {
        for (int i = 0; i < n; i++) {
            float x = mat[0][i];
            float sign = (float) pow(-1.0, 2.0 + i);
            Matrix sub = submatrix(mat, i);
            det += sign * x * det2_seq(sub);
        }
    }
    return det;
}

float det2_par(Matrix mat)
{
    float det = 0;
    int n = mat.size();
    if (n==2) det = det2x2(mat);
    else
    {
#pragma omp parallel for shared(det)
        for (int i = 0; i < n; i++)
        {
            float x = mat[0][i];
            float sign = (float)pow(-1.0, 2.0 + i);
            Matrix sub = submatrix(mat, i);
            det += sign * x * det2_seq(sub);
        }
    }
    return det;
}


int main() {

    Matrix mat;
    /* = {{1.0, 4.0, 8.0, 8.0},
          {2.0, 2.0, 2.0, 8.0},
          {1.0, 2.0, 3.0, 4.0},
          {9.0, 8.0, 7.0, 6.0}};*/
    int size;
    float known_det;
    ifstream in("matrix10x10");
    if (!in) {
        cerr << "File not found" << endl;
        return 0;
    }
    clog << "Загрузка матрицы из файла..." << endl;
    in >> size >> known_det;
    clog << "\tРазмерность = " << size << endl;
    clog << "\tЗаранее вычисленный определитель = " << known_det << endl;


    for (int i = 0; i < size; i++) {
        vector<float> row;
        for (int j = 0; j < size; j++) {
            float element;
            in >> element;
            row.push_back(element);
        }
        mat.push_back(row);
    }
    in.close();

    clog << "Вычисление определителя..." << endl;

    auto start = system_clock::now();
    clog << "\tОпределитель (последовательный) = " << det2_seq(mat) << endl;
    auto end = system_clock::now();
    double delta = duration_cast<milliseconds>(end - start).count() / 1000.0;
    clog << "\tВремя исполнения: " << delta << endl;

    start = system_clock::now();
    clog << "\n\tОпределитель (параллельный) = " << det2_par(mat) << endl;
    end = system_clock::now();
    delta = duration_cast<milliseconds>(end - start).count() / 1000.0 ;
    clog << "\tВремя исполнения: " << delta << endl;

    return 0;
}
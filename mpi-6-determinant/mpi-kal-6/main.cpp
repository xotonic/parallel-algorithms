#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <mpi.h>
#include <stdio.h>
#include  <vector>
#include <fstream>

using namespace std;

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
			float sign = (float)pow(-1.0, 2.0 + i);
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
	if (n == 2) det = det2x2(mat);
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

float checksum(Matrix m)
{
	float sum = 0.0f;
	for (auto row : m)
		for (auto i : row)
			sum += i;
	return sum;
}

float det2_par(Matrix mat, int rank, int total)
{
	float det = 0;
	int n = mat.size();
	if (n == 2) det = det2x2(mat);
	else
	{
//#pragma omp parallel for shared(det)
		for (int i = rank; i < n; i+=total)
		{
			float x = mat[0][i];
			float sign = (float)pow(-1.0, 2.0 + i);
			Matrix sub = submatrix(mat, i);
			det += sign * x * det2_seq(sub);
		}
	}
	return det;
}

int main(int argc, char* argv[]) {

	MPI_Init(&argc, &argv);

	int N, myid, numprocs;
	double startwtime = 0.0, endwtime;
	int namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int _max = 0;

	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Get_processor_name(processor_name, &namelen);
	fprintf(stdout, "Process %d of %d is on %s\n", myid, numprocs, processor_name);
	fflush(stdout);
	float det = -1;
	Matrix mat;
	if (myid == 0)
	{
		int size;
		float known_det;
		ifstream in("matrix10x10");
		if (!in) {
			cerr << "File not found" << endl;
			return 0;
		}
		clog << "Loadning matrix from file..." << endl;
		in >> size >> known_det;
		clog << "\tSize = " << size << endl;
		clog << "\tPrecomputed determinant = " << known_det << endl;

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

		startwtime = MPI_Wtime();
		
		MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

		for (auto i : mat)
		{

			MPI_Bcast(i.data(), i.size(), MPI_FLOAT, 0, MPI_COMM_WORLD);
		}
		det = det2_par(mat, myid, numprocs);
		//fprintf(stdout, "Checksum on %d (main) : %f", myid, checksum(mat));
		//fflush(stdout);
	}
	else
	{
		int size;
		MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

		for (int i = 0; i < size; i++)
		{
			vector<float> row(size);
			MPI_Bcast(row.data(), size, MPI_FLOAT, 0, MPI_COMM_WORLD);
			mat.push_back(row);
		}

		det = det2_par(mat, myid, numprocs);

		//fprintf(stdout, "Checksum on %d : %f", myid, checksum(mat));
		//fflush(stdout);
	}
	float detsum = -1;
	MPI_Reduce(&det, &detsum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
	//MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

	/*
	int max_local;

	max_local = find_local(N, myid, numprocs);
	MPI_Reduce(&max_local, &_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
	*/
	
	if (myid == 0)
	{
		endwtime = MPI_Wtime();
		cout << "Det: " << detsum << endl;
		cout << "Time:   " << endwtime - startwtime << endl;
	}
	
	MPI_Finalize();

	return 0;

}
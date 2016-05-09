#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <mpi.h>
#include <stdio.h>
using namespace std;

bool isPalindrome(int x)
{
	int digit, rev = 0, num = x;

	do
	{
		digit = num % 10;
		rev = (rev * 10) + digit;
		num = num / 10;
	} while (num != 0);

	if (x == rev) return true;
	else return false;
}

int find_local(int initial, int id, int total)
{

	int sroot = sqrt(initial);
	int max_ = 0;

	for (int i = sroot - id; i > 0; i -= total)
	{
		int qsum = i * i;

		for (int j = i - 1; j > 0; j--)
		{
			qsum += j * j;

			if (isPalindrome(qsum) && qsum < initial)
				if (max_ < qsum)
					max_ = qsum;
		}
	}

	return max_;
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

	if (myid == 0)
	{
		fprintf(stdout, "N:");
		fflush(stdout);
		scanf("%d", &N);
		startwtime = MPI_Wtime();
	}

	MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	int max_local;
	
	max_local = find_local(N, myid, numprocs);
	MPI_Reduce( &max_local, &_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

	if (myid == 0)
	{
		endwtime = MPI_Wtime();
		cout << "Number: " << _max << endl;
		cout << "Time:   " << endwtime - startwtime << endl;
	}

	MPI_Finalize();

	return 0;

}
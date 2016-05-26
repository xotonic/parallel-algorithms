//35 „исло называетс€ циркул€рно простым, если люба€ кругова€ перестановка его цифр 
//есть простое число (например Ц 197 или 31 или 17). Ќайти количество циркул€рно простых чисел в диапазоне от N до M

#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <ctime>
#include <mpi.h>
#include <iostream>
#include <cmath>
#define TAG_SUCCESS 97
#define TAG_CUR 96
#define TAG_PREV 95
#define TAG_INITIATIVE 94
#define TAG_STAND 93
using namespace std;

typedef unsigned long long big;
big isEven(big number)
{
	if (number % 2 == 0) return number / 2;
	else return 0;
}

big mul(big a, big b, big m)
{
	if (b == 1)	return a;
	if (b % 2 == 0)
	{
		big t = mul(a, b / 2, m);
		return (2 * t) % m;
	}
	return (mul(a, b - 1, m) + a) % m;
}

big pows(big a, big b, big m)
{
	if (b == 0)	return 1;
	if (b % 2 == 0)
	{
		big t = pows(a, b / 2, m);
		return mul(t, t, m) % m;
	}
	return (mul(pows(a, b - 1, m), a, m)) % m;
}

big gcd(big a, big b)
{
	if (b == 0)	return a;
	return gcd(b, a%b);
}

//////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	int myid;
	int numprocs;
	big t;
	auto startwtime = 0.0;
	auto endwtime = 0.0;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	if (myid == 0)
	{
		cout << "vvedite chislo\nhint: try 8789451874165\n";
		cin >> t;
		cout << "\nWork..." << endl << t << " = ";
		startwtime = MPI_Wtime();
	}
	
	MPI_Bcast(&t, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);

	big temp = 1;

	while (temp != 0)
	{
		temp = isEven(t);
		if (temp != 0)
		{
			t = temp;
			cout << 2;
			if (t > 1) cout << "*";
		}
	}
	big i = 3;

	while (i <= t)
	{
		int allres = 0;
		int res = 0;

		//if (t != 2)
		//{
		srand(time(NULL));
		//int res = 0;
		big a;
		//MPI_Bcast(&t, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
		for (int j = myid; j < 100; j += numprocs)
		{
			a = (rand() % (t - 2)) + 2;
			if (gcd(a, t) != 1)	res++;
			if (pows(a, t - 1, t) != 1)	res++;
		}
		MPI_Reduce(&res, &allres, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
		//cout << endl<< "////////" << allres << "////////" << endl;
		//system("PAUSE");
		if (allres == 0)	//return true;
							//else return false;
							//}

							//bool ferma_res = ferma(t, numprocs, myid);
							//MPI_Gather(&ferma_res/, 1, MPI_UNSIGNED_LONG_LONG, primes, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
							//if (ferma(t, numprocs, myid) == true)
		{
			cout << t;
			break;
		}
		else
		{
			if (t % i == 0)
			{
				cout << i;
				t /= i;
				if (t > 1) cout << "*";
			}
			else i += 2;
		}
	}
	if (myid == 0)
	{
		endwtime = MPI_Wtime();
		clog << endl << "Total: " << (endwtime - startwtime) * 1000 << " ms" << endl;
	}

	MPI_Finalize();
	system("PAUSE");
	return 0;
}
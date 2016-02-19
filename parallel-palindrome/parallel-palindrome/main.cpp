#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <math.h>
#include <stdlib.h>
#include <omp.h>
using namespace std;
using namespace std::chrono;;

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

int sequential(int initial)
{
	int sroot = sqrt(initial);
	int max_ = 0;

	for (int i = sroot; i > 0; i--)
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

int parallel(int initial)
{
	int sroot = sqrt(initial);
	int thread_num = omp_get_max_threads();
	int* maximums = new int[thread_num];
	//cout << "Thread count : " << thread_num << endl;
	//int* loops = new int[thread_num];
	//clear memory
	for (int i = 0; i < thread_num; i++)
	{
		maximums[i] = 0;
		//loops[i] = 0;
	}
#pragma omp parallel
	{
		int id = omp_get_thread_num();
		int total = omp_get_num_threads();
		int part_max = 0;
		//int id_loop = 0;
		for (int i = sroot / total * id; i > sroot / total * (id - 1); i--)
		{
			int qsum = i*i;
			//id_loop++;
			for (int j = i - 1; j > 0; j--)
			{
				qsum += j*j;
				if (isPalindrome(qsum) && qsum < initial)
					if (part_max < qsum) part_max = qsum;
			}
		}
		//loops[id] = id_loop;
		maximums[id] = part_max;
	}

	//cout << "Loops : ";
	//for (int i = 0; i < thread_num; i++)
	//{
	//	cout << loops[i] << ' ';
	//}
	//cout << endl;

	int max_of_max = 0;
	//cout << "Part maximums : ";
	for (int i = 0; i < thread_num; i++)
	{
		//cout << maximums[i] << ' ';
		if (max_of_max < maximums[i]) max_of_max = maximums[i];
	}
	//cout << endl;
	//delete[] maximums;
	return max_of_max;
}

int compute(int init, bool openmp)
{
	int result = -1;
	if (openmp)
		result = parallel(init);
	else
		result = sequential(init);
	return result;
}

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		cout << "Too few arguments" << endl;
		return 1;
	}

	bool openmp = false;

	string file = argv[1];
	string out_file = argv[2];
	if (argc > 3)
	{
		string second = argv[3];
		if (second == "-openmp") openmp = true;
	}

	ifstream fin(file);
	ofstream fout(out_file);
	if (!fin) { cout << "Open file error : " << file << endl; return 1; }
	if (!fout) { cout << "Open file error : " << out_file << endl; return 1; }
	int count = 0;
	fin >> count;
	fout << count << endl;
	auto compute_time = duration<float>::zero();
	
	for (int i = 0; i < count; i++)
	{
		int init = 0;
		fin >> init;
		if (init > 0)
		{
			auto start = system_clock::now();
			int result = compute(init, openmp);
			auto stop = system_clock::now();
			auto delta = stop - start;
			compute_time += delta;
			fout << result << ' ';
			//cout << init << " > \t" << result << endl;
		}
	}
	
	float time_millis = duration_cast<milliseconds>(compute_time).count() / 100.0f;
	cout << "Compute time : " << time_millis << endl;
	fout.close();
	fin.close();

	system("pause");
	return 0;

}

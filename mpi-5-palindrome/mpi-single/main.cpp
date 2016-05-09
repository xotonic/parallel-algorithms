#include <iostream>
#include <omp.h>
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

int find(int initial)
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


int main(int argc, char* argv[]) {
	int N;
	cout << "N: ";
	cin >> N;

	double start1 = omp_get_wtime();
	
	int result = find(N);
	
	double finish1 = omp_get_wtime();
	cout << "Number: " << result << endl;
	cout << "Time: " << finish1 - start1  << endl;

	system("PAUSE");
	return 0;

}
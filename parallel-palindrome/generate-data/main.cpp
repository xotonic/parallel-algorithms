#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
using namespace std;

const int rand_max = 99999;
const int rand_min = 1000;

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		cout << "Too few arguments" << endl;
		return 1;
	}

	string file = argv[1];
	int count = 0;
	stringstream s(argv[2]);
	s >> count;

	ofstream fout(file);
	fout << count << endl;
	cout << "Generating " << count << " numbers..." << endl;
	for (int i = 0; i < count; i++)
	{
		int x = rand_min + rand() % (rand_max - rand_min);
		fout << x << ' ';
	}
	fout.close();
	cout << "Done!" << endl;
	return 0;
}
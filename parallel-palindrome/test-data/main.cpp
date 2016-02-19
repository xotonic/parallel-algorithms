#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
using namespace std;


int main(int argc, char **argv)
{
	if (argc < 3)
	{
		cout << "Too few arguments" << endl;
		return 1;
	}

	string file_path_1 = argv[1];
	string file_path_2 = argv[2];

	ifstream file_1(file_path_1), file_2(file_path_2);
	if (!file_1)
	{
		cout << "Error open file " << file_path_1 << endl; return 1;
	}
	if (!file_2)
	{
		cout << "Error open file " << file_path_2 << endl; return 1;
	}

	int count_1 = 0, count_2 = 0;

	file_1 >> count_1;
	file_2 >> count_2;

	bool all_good = true;
	if (count_1 == count_2)
		for (int i = 0; i < count_1; i++)
		{
			int x = 0, y = 0;
			file_1 >> x;
			file_2 >> y;
			if (x != y)
			{
				cout << "ERROR :\n\t"
					<< file_path_1 << " : " << x << "\n\t"
					<< file_path_2 << " : " << y << "\n";
				all_good = false;
			}
		}
	else cout << "Numbers counts not equal" << endl;
	
	if (all_good) cout << "No errors :3" << endl;
	file_1.close();
	file_2.close();
	return 0;
}
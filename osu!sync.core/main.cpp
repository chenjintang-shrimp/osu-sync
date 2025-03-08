#include<iostream>
#include<fstream>
#include<string>
#include"stableExporter.h"
#include"network.utils.h"
using namespace std;
namespace fs = filesystem;

int main(int argc,char* argv[])
{
	ifstream help(".\\messageFiles\\zh-cn\\usage.txt", ios::binary);
	string helpContent((istreambuf_iterator<char>(help)), istreambuf_iterator<char>());
	if (argc < 2)
	{
		cout << helpContent << endl;
		return 0;
	}
	for (int i = 1;i < argc;i++);
}
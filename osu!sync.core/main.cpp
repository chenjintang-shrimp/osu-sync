#include<iostream>
#include<fstream>
#include<map>
#include<string>
#include"stableExporter.h"
#include"network.utils.h"
#include"configs.h"
using namespace std;
namespace fs = filesystem;

int main(int argc,char* argv[])
{
	ifstream help(".\\messageFiles\\zh-cn\\usage.txt", ios::binary);
	string helpContent((istreambuf_iterator<char>(help)), istreambuf_iterator<char>());
	try
	{
		uint32_t paramFlags;
		paramList_t pmList;
		if (argc == 0)
			throw invalid_argument("请提供选项！\n Please provide a argument.\n");
		bool invalidArg = false;
		string invalidArgValue;
		for (int i = 1;i < argc;i++)
		{
			if (paramMapper.count(argv[i]))
			{
				
			}
		}
	}
	catch (const invalid_argument& ie)
	{
		cout << ie.what() << endl << endl;
		cout << helpContent << endl;
		return 0;
	}
	return 0;
}
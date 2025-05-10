#include<iostream>
#include<algorithm>
#include<filesystem>
#include"uploadArchive.h"

using namespace std;
namespace fs = std::filesystem;

//#define else if elif

int main(int argc,char* argv[])
{
	if (argc > 1)
	{
		string firstArg = argv[1];
		if (firstArg == "--version")
			cout << "osu!sync core, version b-20250501" << endl;
		else if (firstArg == "--help")
			cout << "用法：osu!sync.core.exe [参数]" << endl;
		else if (firstArg == "--upload")
		{
			string username = argv[2];
			string archiveFileName = argv[3];
			string serverURL = argv[4];
			uploadData(archiveFileName, username, serverURL);
		}
	}
}
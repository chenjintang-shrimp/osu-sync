#include<iostream>
#include<algorithm>
#include<filesystem>
#include"fsutils.windows.h"
#include"network.utils.h"
#include"stableExporter.h"
using namespace std;
namespace fs = std::filesystem;

#define else if elif

int main(int argc,char* argv[])
{
	if (argc == 5)
	{
		fs::path osuFolderPath;
		string options,serverURL;
		osuFolderPath = argv[1], options = argv[2];
	}
	else
	{
		cout << "ÓÃ·¨£º" << endl
			<< "osu!sync.core.exe [osu-folder] [download|upload|export|import] [serverURL]" << endl;
		return -1;
	}
}
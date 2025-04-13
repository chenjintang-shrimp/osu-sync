#include<iostream>
#include<algorithm>
#include<filesystem>
#include"fsutils.windows.h"
#include"network.utils.h"
#include"stableExporter.h"
using namespace std;
namespace fs = std::filesystem;

//#define else if elif

int main(int argc,char* argv[])
{
	if (argc == 5)
	{
		fs::path osuFolderPath;
		string options,serverURL,username;
		osuFolderPath = argv[1], username = argv[2],options=argv[3],serverURL=argv[4];
		if (options == "download")
		{
			downloadBeatmapFromList(username + "_stableBeatmaps.json",".\\tempdownload");
		}
		else if (options == "upload")
		{
			uploadData(username + "_stableBeatmaps.json", username, serverURL);
		}
		else if (options == "import")
		{

		}
		else if (options == "export")
		{
			writeBeatmapDataToFile(getAllBeatmapData(getBeatmapFolderNames(osuFolderPath /= "songs")),username+"_stableBeatmaps.json");
		}
	}
	else
	{
		cout << "ÓÃ·¨£º" << endl
			<< "osu!sync.core.exe [osu-folder] [username] [download|upload|export|import] [serverURL]" << endl;
		return -1;
	}
}
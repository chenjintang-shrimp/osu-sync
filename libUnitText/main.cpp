#include<iostream>
#include<fstream>
#include<filesystem>
#include"stableExporter.h"
#include"fsutils.windows.h"
#include"network.utils.h"
using namespace std;

int main()
{
	string pathStr;
	cin >> pathStr;
	fs::path songsPath = pathStr;
	vector<beatmapMetaData> bmd;
	bmd = getAllBeatmapData(getBeatmapFolderNames(songsPath));
	writeBeatmapDataToFile(bmd,".\\test.json");
	uploadData(".\\test.json", "test", "120.26.51.57:21");
	return 0;
}
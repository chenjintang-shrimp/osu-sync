#include<iostream>
#include<fstream>
#include<filesystem>
#include"stableExporter.h"
#include"fsutils.windows.h"
using namespace std;

int main()
{
	string pathStr;
	cin >> pathStr;
	fs::path songsPath = pathStr;
	vector<beatmapMetaData> bmd;
	bmd = getAllBeatmapData(getBeatmapFolderNames(songsPath));
	writeBeatmapDataToFile(bmd,".\\test.json");
	return 0;
}
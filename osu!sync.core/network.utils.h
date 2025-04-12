#include<iostream>
#include<fstream>
#include<filesystem>
#include"3rdpartyInclude/httplib.h"
#include"stableExporter.h"
#define NOSSL

namespace fs = std::filesystem;

int uploadData(fs::path filename, std::string username, std::string remoteURL);
int downloadBeatmapList(fs::path downloadFileName, std::string username, std::string remoteURL);
//返回的是以清单顺序排序的osz名字列表，格式为 "谱面id".osz
std::vector<std::string> downloadBeatmapFromList(fs::path pathToBeatmapList);

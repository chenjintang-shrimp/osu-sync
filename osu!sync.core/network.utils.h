#include<iostream>
#include<fstream>
#include<filesystem>
#include"3rdpartyInclude/httplib.h"
#include"stableExporter.h"
#define NOSSL

namespace fs = std::filesystem;

int uploadData(fs::path filename, std::string username, std::string remoteURL);
int downloadBeatmapList(fs::path downloadFileName, std::string username, std::string remoteURL);
//���ص������嵥˳�������osz�����б���ʽΪ "����id".osz
std::vector<std::string> downloadBeatmapFromList(fs::path pathToBeatmapList);
